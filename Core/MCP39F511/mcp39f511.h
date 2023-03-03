/*
 * mcp39f511.h
 *
 *  Created on: Mar 2, 2023
 *      Author: Michal Klebokowski
 */
#ifndef MCP39F511_MCP39F511_H_
#define MCP39F511_MCP39F511_H_

#include "main.h"
#include "../RingBuffer/RingBuffer.h"


//
//MCP39F511 Instruction set
//
#define READ_REGISTER 0x4E
#define WRITE_REGISTER 0x4D
#define SET_ADDR_PTR 0x41
#define SAVE_REG_TO_FLASH 0x53
#define READ_EEPROM_PAGE 0x42
#define WRITE_EEPROM_PAGE 0x50
#define EREASE_EEPROM 0x4F
#define AUTO_CALIBRATE_GAIN 0x5A
#define AUTO_CALIBRATE_REACTIVE_GAIN 0x7A
#define AUTO_CALIBRATE_FREQENCY 0x76

//
//MCP39F511 Readable Register List
//

#define SYSTEM_STATUS 0x0002
#define SYSTEM_VERSION 0x0004
#define VOLTAGE_RMS 0x0006
#define LINE_FREQUENCY 0x0008
#define ADC_VOLTAGE 0x000A
#define POWER_FACTOR 0x000C
#define CURRENT_RMS 0x000E
#define ACTIVE_POWER 0x0012
#define REACTIVE_POWER 0x0016
#define APPARENT_POWER 0x001A
#define IMPORT_ACTIVE_ENERGY_COUNTER 0x001E
#define EXPORT_ACTIVE_ENERGY_COUNTER 0x0026
#define IMPORT_REACTIVE_ENERGY_COUNTER 0x002E
#define EXPORT_REACTIVE_ENERGY_COUNTER 0x0036
#define MINIMUM_RECORD_1 0x003E
#define MINIMUM_RECORD_2 0x0043
#define MAXIMUM_RECORD_1 0x004E
#define MAXIMUM_RECORD_2 0x0052


#define HEADER_BYTE 0xA5

typedef struct{
	UART_HandleTypeDef* UART;

	uint8_t DataSendBuffer[35];
	uint8_t TmpReceiveBuffer[32];
	RBuffer_t ReceivedDataBuffer;

	uint16_t VoltageRMS;
	uint32_t CurrentRMS;
	uint16_t LineFrequency;
	int16_t PowerFactor;
}MCP39F511_t;

void MCP39F511_Init(MCP39F511_t *IC, UART_HandleTypeDef *huart);						//Init function
void MCP39F511_ReadData(MCP39F511_t *IC, uint16_t StartAddres, uint8_t NumberOfBytes); 	//Get data


#endif /* MCP39F511_MCP39F511_H_ */
