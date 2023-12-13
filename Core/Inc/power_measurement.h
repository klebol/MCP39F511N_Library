/*
 * power_measurement.h
 *
 *  Created on: Jun 7, 2023
 *      Author: Michal Klebokowski
 */

#ifndef INC_POWER_MEASUREMENT_H_
#define INC_POWER_MEASUREMENT_H_

//#include "main.h"
#include "mcp39f511n.h"


#define ASK_DELAY_MS 200			//Odstęp czasowy pomiedzy kolejnymi zapytaniami o parametry sieci


typedef struct
{
	uint32_t ActivePower;
	float CurrentRMS;
	uint16_t VoltageRMS;
	uint8_t Error;
}AC_Parameters_t;

#define COMMUNICATION_TIMEOUT 500	//Czas oczekiwania na odpowiedź, po jego upłunięciu zostaje podjęta kolejna próba
#define MAX_RETRIES 20				//Maksymalna liczba prób ponownej komunikacji, przed wysłaniem komunikatu o błędzie


void PowerMeasurement_Init(UART_HandleTypeDef *huart);
void PowerMeasurement_Process(AC_Parameters_t *ParametersOutput);
void PowerMeasurement_RX_Callback(UART_HandleTypeDef *huart);

#endif /* INC_POWER_MEASUREMENT_H_ */
