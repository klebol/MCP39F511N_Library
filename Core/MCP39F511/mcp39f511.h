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



#endif /* MCP39F511_MCP39F511_H_ */
