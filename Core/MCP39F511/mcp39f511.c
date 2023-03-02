/*
 * mcp39f511.c
 *
 *  Created on: Mar 2, 2023
 *      Author: Michal Klebokowski
 *
 */
#include "mcp39f511.h"
#include "string.h"

//czy na pewno ring buffer? jak duzo danych bedzie przychodzic

void MCP39F511_Init(MCP39F511_t *IC, UART_HandleTypeDef *huart)
{
	IC->UART = huart; //define mcp's uart
	HAL_UARTEx_ReceiveToIdle_DMA(huart,(IC->TmpReceiveBuffer),32);
}

//
//
//

void MCP39F511_SendData(MCP39F511_t *IC)	//basic send function
{
	HAL_UART_Transmit_DMA(IC->UART, IC->DataSendBuffer, sizeof(IC->DataSendBuffer));
}

void MCP39F511_ListenDMA(MCP39F511_t *IC)
{
	HAL_UARTEx_ReceiveToIdle_DMA(IC -> UART,IC->TmpReceiveBuffer,32);
}

void  MCP39F511_WriteToReceiveRingBuffer(MCP39F511_t *IC, uint8_t Length)	//use in DMA Idle callback
{
	uint8_t i;
	for(i = 0; i < Length; i++)
	{
		RBuffer_Write(&(IC->ReceivedDataBuffer), IC->TmpReceiveBuffer[i]);
	}
	//memset (clear tmp buffer)
}


void MCP39F511_ReadReceiveBuffer(MCP39F511_t *IC)
{

}

