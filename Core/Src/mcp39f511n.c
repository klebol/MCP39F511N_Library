/*
 * mcp39f511n.c
 *
 *  Created on: Jun 7, 2023
 *      Author: Michal Klebokowski
 */
#include "mcp39f511n.h"

//
//Init
//

void PowerMonitor_Init(PowerMonitor_t *IC, UART_HandleTypeDef *huart)
{
	IC->UART = huart; //define mcp's uart
}

//
//Basic read - write functions (with DMA)
//

PowerMonitor_Status_t PowerMonitor_SendData(PowerMonitor_t *IC, uint8_t *Data, uint8_t Size)
{
	return HAL_UART_Transmit_DMA(IC->UART, Data, Size);
}

PowerMonitor_Status_t PowerMonitor_ListenDMA(PowerMonitor_t *IC, uint8_t *Destination)
{
	return HAL_UARTEx_ReceiveToIdle_DMA(IC -> UART, Destination, 35);
}

//
//Checksum calculation
//

uint8_t PowerMonitor_CalculateChecksum(uint8_t *FrameForCalculate, uint8_t NumberOfBytes)
{
	    uint16_t Checksum = 0;
	    uint8_t i = 1;
	    uint8_t *ByteToAdd = FrameForCalculate;
	    do {
	        Checksum += *(ByteToAdd++);
	        i++;
	    } while (i <= NumberOfBytes);
	    Checksum %= 0x0100;
	    return(Checksum);
}

//
//Read - write with full frame generation
//

PowerMonitor_Status_t PowerMonitor_ReadData(PowerMonitor_t *IC, uint8_t *Destination, uint16_t StartAddress, uint8_t NumberOfBytes)
{
	uint8_t Status = 0;
	IC->SendBuffer[0] = HEADER_BYTE;										//frame header byte
	IC->SendBuffer[1] = 8; 													//number of all bytes in frame
	IC->SendBuffer[2] = SET_ADDR_PTR;										//set address pointer do desired data
	IC->SendBuffer[3] = (StartAddress >> 8);								//address HIGH
	IC->SendBuffer[4] = StartAddress;										//address LOW
	IC->SendBuffer[5] = READ_REGISTER;										//read register command
	IC->SendBuffer[6] = NumberOfBytes;										//number of bytes to read
	IC->SendBuffer[7] = PowerMonitor_CalculateChecksum(IC->SendBuffer, 7);	//checksum
	Status = PowerMonitor_SendData(IC, IC->SendBuffer, 8);					//send function
	Status = PowerMonitor_ListenDMA(IC, Destination);						//set DMA to listen
	return Status;
}

PowerMonitor_Status_t PowerMonitor_WriteData(PowerMonitor_t *IC, uint8_t *DataToWrite, uint16_t StartAddress, uint8_t NumberOfBytes)	//TODO: test it
{
	uint8_t Status = 0;
	IC->SendBuffer[0] = HEADER_BYTE;						//frame header byte
	IC->SendBuffer[1] = 8+NumberOfBytes; 					//number of all bytes in frame
	IC->SendBuffer[2] = SET_ADDR_PTR;						//set address pointer do desired data
	IC->SendBuffer[3] = (StartAddress >> 8);				//address HIGH
	IC->SendBuffer[4] = StartAddress;						//address LOW
	IC->SendBuffer[5] = WRITE_REGISTER;						//write register command
	IC->SendBuffer[6] = NumberOfBytes;						//number of bytes to write
	for(uint8_t i = 0; i < NumberOfBytes; i++)
	{
		IC->SendBuffer[7+i] = *(DataToWrite+i);
	}
	IC->SendBuffer[7 + NumberOfBytes] = PowerMonitor_CalculateChecksum(IC->SendBuffer, 6 + NumberOfBytes);
	Status = PowerMonitor_SendData(IC, IC->SendBuffer, 8);			//send function
//	MCP39F511_ListenDMA(IC);								//set DMA to listen
	return Status;
}

//
//Parse functions
//

void PowerMonitor_ParseBasics(PowerMonitor_t *IC, uint8_t *Data)
{
	uint32_t Tmp;
	uint8_t *DataPointer = Data;
	//Get Voltage
	IC->VoltageRMS =  (DataPointer[0] | (DataPointer[1] << 8)) / CALC_VOLTAGE_DIVIDER;
	//Get Frequency
	Tmp = DataPointer[2] | (DataPointer[3] << 8);
	IC->LineFrequency = (float)Tmp / CALC_FREQUENCY_DIVIDER;
	//Get Power Factor 1
	IC->Channel1.PowerFactor =  DataPointer[4] | (DataPointer[5] << 8);
	//Get Power Factor 2
	IC->Channel2.PowerFactor =  DataPointer[6] | (DataPointer[7] << 8);
	//Get Current CH1
	//IC->Channel1.CurrentRMS = (DataPointer[8] | (DataPointer[9] << 8) | (DataPointer[10] << 16) | (DataPointer[11] << 24) ) / CALC_CURRENT_DIVIDER;
	Tmp = (DataPointer[8] | (DataPointer[9] << 8) | (DataPointer[10] << 16) | (DataPointer[11] << 24) );
	IC->Channel1.CurrentRMS = (float)Tmp / CALC_CURRENT_DIVIDER;
	//Get Current CH2
	//IC->Channel2.CurrentRMS = (DataPointer[12] | (DataPointer[13] << 8) | (DataPointer[14] << 16) | (DataPointer[15] << 24) ) / CALC_CURRENT_DIVIDER;
	Tmp = (DataPointer[12] | (DataPointer[13] << 8) | (DataPointer[14] << 16) | (DataPointer[15] << 24) );
	IC->Channel2.CurrentRMS = (float)Tmp / CALC_CURRENT_DIVIDER;
}

void PowerMonitor_ParsePowers(PowerMonitor_t *IC, uint8_t *Data)
{
	uint8_t *DataPointer = Data;
	//Get Active Power 1
	IC->Channel1.ActivePower = (DataPointer[0] | (DataPointer[1] << 8) | (DataPointer[2] << 16) | (DataPointer[3] << 24) ) / CALC_POWER_DIVIDER;
	//Get Active Power 2
	IC->Channel2.ActivePower = (DataPointer[4] | (DataPointer[5] << 8) | (DataPointer[6] << 16) | (DataPointer[7] << 24) ) / CALC_POWER_DIVIDER;
	//Get Reactive Power 1
	IC->Channel1.ReactivePower = DataPointer[8] | (DataPointer[9] << 8) | (DataPointer[10] << 16) | (DataPointer[11] << 24);
	//Get Reactive Power 2
	IC->Channel2.ReactivePower = DataPointer[12] | (DataPointer[13] << 8) | (DataPointer[14] << 16) | (DataPointer[15] << 24);
	//Get Apparent Power 1
	IC->Channel1.ApparentPower = DataPointer[16] | (DataPointer[17] << 8) | (DataPointer[18] << 16) | (DataPointer[19] << 24);
	//Get Apparent Power 1
	IC->Channel2.ApparentPower = DataPointer[20] | (DataPointer[21] << 8) | (DataPointer[22] << 16) | (DataPointer[23] << 24);
}


PowerMonitor_Status_t PowerMonitor_Parse(PowerMonitor_t *IC)
{
	uint16_t Tmp;
	uint8_t *Byte = IC->ReceiveBuffer;

	if(*Byte == 0x00)
	{
		return POWER_MONITOR_BUSY; 	//no new frame
	}

	if(*Byte == MCP_NACK)
	{
		return POWER_MONITOR_ERROR;	//NACK received, error
	}
	else if(*Byte == MCP_ACK)
	{
		Byte+=2;					//Go to data in frame (skip number of bytes byte)
	}

	switch(IC->ExpectedData)
	{
//	Reading all parameters:
	case READ_ALL_BASICS:
		PowerMonitor_ParseBasics(IC, Byte);
		break;

	case READ_ALL_POWERS:
		PowerMonitor_ParsePowers(IC, Byte);
		break;
//	Reading by one parameter:
	case VOLTAGE_RMS:
		IC->VoltageRMS =  Byte[0] | (Byte[1] << 8);
		break;

	case LINE_FREQUENCY:
		Tmp = Byte[0] | (Byte[1] << 8);
		IC->LineFrequency = (float)Tmp / CALC_FREQUENCY_DIVIDER;
		break;

	case POWER_FACTOR_1:
		IC->Channel1.PowerFactor =  Byte[0] | (Byte[1] << 8);
		break;

	case POWER_FACTOR_2:
		IC->Channel2.PowerFactor =  Byte[0] | (Byte[1] << 8);
		break;

	case CURRENT_RMS_1:
		IC->Channel1.CurrentRMS = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	case CURRENT_RMS_2:
		IC->Channel2.CurrentRMS = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	case ACTIVE_POWER_1:
		IC->Channel1.ActivePower = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	case ACTIVE_POWER_2:
		IC->Channel2.ActivePower = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	case REACTIVE_POWER_1:
		IC->Channel1.ReactivePower = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	case REACTIVE_POWER_2:
		IC->Channel2.ReactivePower = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	case APPARENT_POWER_1:
		IC->Channel1.ApparentPower = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	case APPARENT_POWER_2:
		IC->Channel2.ApparentPower = Byte[0] | (Byte[1] << 8) | (Byte[2] << 16) | (Byte[3] << 24);
		break;

	default:
		break;
	}

	memset(IC->ReceiveBuffer,0x00,32);		//clear parsed frame
	IC->ExpectedData = 0;
	return POWER_MONITOR_OK;
}


//
//Specific read functions
//

void PowerMonitor_ReadVoltege(PowerMonitor_t *IC)
{
	if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, VOLTAGE_RMS, 2) )
	{
		IC->ExpectedData = VOLTAGE_RMS;
	}
}

void PowerMonitor_ReadFrequency(PowerMonitor_t *IC)
{
	if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, LINE_FREQUENCY, 2) )
	{
		IC->ExpectedData = LINE_FREQUENCY;
	}
}

void PowerMonitor_ReadPowerFactor(PowerMonitor_t *IC, uint8_t Channel)
{
	if(Channel == 1)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, POWER_FACTOR_1, 2) )
		{
			IC->ExpectedData = POWER_FACTOR_1;
		}
	}
	else if(Channel == 2)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, POWER_FACTOR_2, 2) )
		{
			IC->ExpectedData = POWER_FACTOR_2;
		}
	}

}

void PowerMonitor_ReadCurrentRMS(PowerMonitor_t *IC, uint8_t Channel)
{
	if(Channel == 1)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, CURRENT_RMS_1, 4) )
		{
			IC->ExpectedData = CURRENT_RMS_1;
		}
	}
	else if(Channel == 2)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, CURRENT_RMS_2, 4) )
		{
			IC->ExpectedData = CURRENT_RMS_2;
		}
	}
}

void PowerMonitor_ReadActivePower(PowerMonitor_t *IC, uint8_t Channel)
{
	if(Channel == 1)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, ACTIVE_POWER_1, 4) )
		{
			IC->ExpectedData = ACTIVE_POWER_1;
		}
	}
	else if(Channel == 2)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, ACTIVE_POWER_2, 4) )
		{
			IC->ExpectedData = ACTIVE_POWER_2;
		}
	}
}

void PowerMonitor_ReadReactivePower(PowerMonitor_t *IC, uint8_t Channel)
{
	if(Channel == 1)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, REACTIVE_POWER_1, 4) )
		{
			IC->ExpectedData = REACTIVE_POWER_1;
		}
	}
	else if(Channel == 2)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, REACTIVE_POWER_2, 4) )
		{
			IC->ExpectedData = REACTIVE_POWER_2;
		}
	}
}

void PowerMonitor_ReadApparentPower(PowerMonitor_t *IC, uint8_t Channel)
{
	if(Channel == 1)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, APPARENT_POWER_1, 4) )
		{
			IC->ExpectedData = APPARENT_POWER_1;
		}
	}
	else if(Channel == 2)
	{
		if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, APPARENT_POWER_2, 4) )
		{
			IC->ExpectedData = APPARENT_POWER_2;
		}
	}
}

void PowerMonitor_ReadBasics(PowerMonitor_t *IC)
{
	if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, VOLTAGE_RMS, 16) )
	{
		IC->ExpectedData = READ_ALL_BASICS;
	}
}

void PowerMonitor_ReadPowers(PowerMonitor_t *IC)
{
	if(POWER_MONITOR_OK == PowerMonitor_ReadData(IC, IC->ReceiveBuffer, ACTIVE_POWER_1, 24) )
	{
		IC->ExpectedData = READ_ALL_POWERS;
	}
}
