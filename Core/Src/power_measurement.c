/*
 * power_measurement.c
 *
 *  Created on: Jun 7, 2023
 *      Author: Michal Klebokowski
 */

#include "power_measurement.h"

PowerMonitor_t PowerStripIC;
uint8_t DataInProgressFlag;
volatile uint8_t DataReceivedFlag;

uint32_t LastTick_Timeout = 0;
uint8_t TimeoutRetries;
uint8_t NackRetries;

void PowerMeasurement_Init(UART_HandleTypeDef *huart)
{
	PowerMonitor_Init(&PowerStripIC, huart);
}


void PowerMeasurement_Process(AC_Parameters_t *ParametersOutput)			//Proces komunikacji z scalakiem pomiarowym (MCP39F511N)
{
	static uint8_t ReadOrder = 0;
	static uint32_t LastTick_Delay = 0;											//Tick sluzacy do odmierzania odstepu miedzy zapytaniami
	uint32_t CurrentTick;														//Aktualny tick pobierany przy kazdorazowym wejsciu w ten process
	uint8_t Status;

	CurrentTick = HAL_GetTick();

	if((CurrentTick - LastTick_Delay > ASK_DELAY_MS) && DataInProgressFlag == 0)	//Sprzawdz czy uplynal odpowiedni czas od ostatniego zapytania, oraz czy aktualnie nie czekamy
	{																			//na jakąś ramkę bądź nie parsujemy już jakichś danych (DataInProgressFlag zerowana jest po
		switch(ReadOrder)														//przetworzeniu odebranej ramki
		{
		case 0:																	//Zmienna ReadOrder odpowiada za wysyłanie naprzemiennie dwóch rodzajów zapytań
			PowerMonitor_ReadBasics(&PowerStripIC);								//Zapytanie o basics (napięcie, czestotliwosc, prąd)
			ReadOrder = 1;														//Zmiana ReadOrder by w następnym wejściu zapytać o Powers
			break;
		case 1:
			PowerMonitor_ReadPowers(&PowerStripIC);								//Zapytanie o Powers (wszystkie rodzaje mocy)
			ReadOrder = 0;														//Zmiana ReadOrder
			break;
		}
		DataInProgressFlag = 1;													//gdy wyślemy zapytanie, ustawiamy DataInProgressFlag na 1 by nie wysyłać kolejnego
		LastTick_Delay = HAL_GetTick();											//Zapisujemy last tick by odmierzac znow czas odstepu miedzy zapytaniami
		LastTick_Timeout = HAL_GetTick();										//Zapisujemy last tick do odmierzania timeout w oczekiwaniu na odpowiedz
	}

	if(1 == DataReceivedFlag)													//Tu wchodzimy w celu parsowania otrzymanej ramki. DataReceivedFlag ustawiane jest jest
	{																			//przez przerwanie w w funkcji RX_Callback
		Status = PowerMonitor_Parse(&PowerStripIC);
		if(POWER_MONITOR_OK == Status)											//jesli parsowanie sie powiodlo, wypluwami zmienne na zewnatrz. to tutaj decydujemy jakie
		{																		//dane sa udostepnianie, PowerStripIC czyta wszystko z MCP39F511N
			ParametersOutput->VoltageRMS = PowerStripIC.VoltageRMS;
			ParametersOutput->ActivePower = PowerStripIC.Channel1.ActivePower + PowerStripIC.Channel2.ActivePower;
			ParametersOutput->CurrentRMS = PowerStripIC.Channel1.CurrentRMS + PowerStripIC.Channel2.CurrentRMS;

			if(ParametersOutput->Error == 0x01)					//usuń error jeśli udało się odnowić komunikaje po jakims czasie
			{
				ParametersOutput->Error = 0x00;

			}

		}
		else if(POWER_MONITOR_BUSY == Status)									//obsluga przypadku gdy nie mamy zadnej ramki do odebrania (program nigdy nie powinien tu wejsc poniewaz
		{																		//zawsze bedzie ramka do parsowania gdy DataReceivedFlag == 1)

		}
		else if(POWER_MONITOR_ERROR == Status)									//obsluga bledu, gdy MCP39F511N zwroci NACK (bład ramki)
		{
			NackRetries++;
			if(NackRetries >= MAX_RETRIES)
			{
				ParametersOutput->Error = 0x01;
				NackRetries = 0;
			}
		}
		DataInProgressFlag = 0;													//po parsowaniu ramki, wyzeruj obie flagi my moglo zostac wyslane kolejne zapytanie
		DataReceivedFlag = 0;													//oraz by czekac na przybycie kolejnej ramki
		TimeoutRetries = 0;
	}
	else if( (HAL_GetTick() - LastTick_Timeout) > COMMUNICATION_TIMEOUT	)		//obsluga timeoutu komunikacji (gdy odpowiedz nie przyjdzie w czasie okreslonym w makrze
	{																			//COMMUNICATION_TIMEOUT)
		LastTick_Delay = HAL_GetTick();											//Reset ticku odstepu ostatniego zapytania oraz reset flagi DataInProgress w celu
		LastTick_Timeout = HAL_GetTick();
		DataInProgressFlag = 0;													//ponownej proby komunikacji
		TimeoutRetries++;
		if(TimeoutRetries >= MAX_RETRIES)										//po osiagniaciu maksymalnej liczby prob, zglos blad, wyzeruj retries by probowac
		{																		//skomunikowac sie od nowa
			ParametersOutput->Error = 0x01;
			ParametersOutput->VoltageRMS = 0;
			ParametersOutput->ActivePower = 0;
			ParametersOutput->CurrentRMS = 0;
			TimeoutRetries = 0;
		}
	}
}

void PowerMeasurement_RX_Callback(UART_HandleTypeDef *huart)					//callback do umieszczenie w przerwanie UART_RX - IDLE
{
	if(huart == PowerStripIC.UART)
	{
		DataReceivedFlag = 1;

	}
}


