/*
 * mcp39f511n.h
 *
 *  Created on: Jun 7, 2023
 *      Author: Michal Klebokowski
 *
 */

#ifndef INC_MCP39F511N_H_
#define INC_MCP39F511N_H_

#include "main.h"
#include "string.h"

//
//Data calculation multipliers
//
#define CALC_VOLTAGE_DIVIDER	10
#define CALC_FREQUENCY_DIVIDER	1000
#define CALC_CURRENT_DIVIDER	10000
#define CALC_POWER_DIVIDER		100
#define CALC_ENERGY_DIVIDER		1000

//
//MCP39F511N Instruction set
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
//-------
#define HEADER_BYTE 0xA5
#define MCP_ACK 0x06
#define MCP_NACK 0x15

//
//MCP39F511N Readable Register List
//

//SYSTEM INFO
#define INSTRUCTION_POINTER 0x0000
#define SYSTEM_STATUS 0x0002
#define SYSTEM_VERSION 0x0004
//BASIC PARAMETERS
#define VOLTAGE_RMS 0x0006
#define LINE_FREQUENCY 0x0008
#define POWER_FACTOR_1 0x000A
#define POWER_FACTOR_2 0x000C
#define CURRENT_RMS_1 0x000E
#define CURRENT_RMS_2 0x0012
//POWER PARAMETERS
#define ACTIVE_POWER_1 0x0016
#define ACTIVE_POWER_2 0x001A
#define REACTIVE_POWER_1 0x001E
#define REACTIVE_POWER_2 0x0022
#define APPARENT_POWER_1 0x0026
#define APPARENT_POWER_2 0x002A
//IMPORT-EXPORT ENERGY COUNTERS
#define IM_ACTIVE_ENERGY_1 0x002E
#define IM_ACTIVE_ENERGY_2 0x0036
#define EX_ACTIVE_ENERGY_1 0x003E
#define EX_ACTIVE_ENERGY_2 0x0046
#define IM_REACTIVE_ENERGY_1 0x004E
#define IM_REACTIVE_ENERGY_2 0x0056
#define EX_REACTIVE_ENERGY_1 0x005E
#define EX_REACTIVE_ENERGY_2 0x0066

//
//ADDITIONAL COMMUNICATION MACROS
//
#define READ_ALL_BASICS 0xBBBB
#define READ_ALL_POWERS 0xAAAA

//
//TYPES
//

typedef enum{
	POWER_MONITOR_OK,
	POWER_MONITOR_ERROR,
	POWER_MONITOR_BUSY,
	POWER_MONITOR_TIMEOUT
}PowerMonitor_Status_t;

typedef struct{
	int16_t PowerFactor;
	float CurrentRMS;
	uint32_t ActivePower;
	uint32_t ReactivePower;
	uint32_t ApparentPower;
}PowerMonitor_ChannelValues_t;

typedef struct{
	//UART INTERFACE
	UART_HandleTypeDef* UART;
	//COMMUNICATION BUFFERS & FLAGS
	uint8_t SendBuffer[32];
	uint8_t ReceiveBuffer[32];
	uint16_t ExpectedData;
	//LINE PARAMETERS
	uint16_t VoltageRMS;
	float LineFrequency;
	PowerMonitor_ChannelValues_t Channel1;
	PowerMonitor_ChannelValues_t Channel2;
}PowerMonitor_t;

//
//FUNCTIONS
//

//Initialiaton
void PowerMonitor_Init(PowerMonitor_t *IC, UART_HandleTypeDef *huart);
//Function to call in UART RX interrupt
void PowerMonitor_CheckDataReady(PowerMonitor_t *IC, UART_HandleTypeDef *huart);
//Read multiple parameters functions
void PowerMonitor_ReadBasics(PowerMonitor_t *IC);
void PowerMonitor_ReadPowers(PowerMonitor_t *IC);
//Read one parameter funcions
void PowerMonitor_ReadVoltege(PowerMonitor_t *IC);
void PowerMonitor_ReadFrequency(PowerMonitor_t *IC);
void PowerMonitor_ReadPowerFactor(PowerMonitor_t *IC, uint8_t Channel);
void PowerMonitor_ReadCurrentRMS(PowerMonitor_t *IC, uint8_t Channel);
void PowerMonitor_ReadActivePower(PowerMonitor_t *IC, uint8_t Channel);
void PowerMonitor_ReadReactivePower(PowerMonitor_t *IC, uint8_t Channel);
void PowerMonitor_ReadApparentPower(PowerMonitor_t *IC, uint8_t Channel);
//Parser function
PowerMonitor_Status_t PowerMonitor_Parse(PowerMonitor_t *IC);

#endif /* INC_MCP39F511N_H_ */
