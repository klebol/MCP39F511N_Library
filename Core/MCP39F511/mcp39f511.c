/*
 * mcp39f511.c
 *
 *  Created on: Mar 2, 2023
 *      Author: Michal Klebokowski
 *
 */
#include "mcp39f511.h"
#include "string.h"



void MCP39F511_Init(MCP39F511_t *IC, UART_HandleTypeDef *huart)
{
	IC->UART = huart; //define mcp's uart
}

//
//
//

void MCP39F511_SendData(MCP39F511_t *IC, uint8_t Size)	//basic send function
{
	HAL_UART_Transmit_DMA(IC->UART, IC->DataSendBuffer, Size);
}

void MCP39F511_ListenDMA(MCP39F511_t *IC)
{
	HAL_UARTEx_ReceiveToIdle_DMA(IC -> UART,IC->TmpReceiveBuffer,32);
}

//void  MCP39F511_WriteToReceiveRingBuffer(MCP39F511_t *IC, uint8_t Length)	//use in DMA Idle callback
//{
//	uint8_t i;
//	for(i = 0; i < Length; i++)
//	{
//		RBuffer_Write(&(IC->ReceivedDataBuffer), IC->TmpReceiveBuffer[i]);
//	}
//	//memset (clear tmp buffer)
//}


uint8_t MCP39F511_CalculateChecksum(MCP39F511_t *IC, uint8_t NumberOfBytes)
{
	    uint16_t Checksum = 0;
	    uint8_t i = 0;
	    do {
	        Checksum += IC->DataSendBuffer[i];
	        i++;
	    } while (i <= NumberOfBytes);
	    Checksum %= 0x0100;
	    return(Checksum);
}

void MCP39F511_ReadData(MCP39F511_t *IC, uint16_t StartAddress, uint8_t NumberOfBytes)
{
	IC->DataSendBuffer[0] = HEADER_BYTE;				//frame header byte
	IC->DataSendBuffer[1] = 8; 							//number of all bytes in frame
	IC->DataSendBuffer[2] = SET_ADDR_PTR;				//set address pointer do desired data
	IC->DataSendBuffer[3] = (StartAddress >> 8);			//address HIGH
	IC->DataSendBuffer[4] = StartAddress;				//address LOW
	IC->DataSendBuffer[5] = READ_REGISTER;				//read register command
	IC->DataSendBuffer[6] = NumberOfBytes;				//number of bytes to read
	IC->DataSendBuffer[7] = MCP39F511_CalculateChecksum(IC, 6);	//checksum
	MCP39F511_SendData(IC,8);								//send function
	MCP39F511_ListenDMA(IC);							//set DMA to listen
}


void MCP39F511_WriteData(MCP39F511_t *IC, uint16_t StartAddress, uint8_t *DataToWrite, uint8_t NumberOfBytes)
{
	IC->DataSendBuffer[0] = HEADER_BYTE;				//frame header byte
	IC->DataSendBuffer[1] = 8; 							//number of all bytes in frame
	IC->DataSendBuffer[2] = SET_ADDR_PTR;				//set address pointer do desired data
	IC->DataSendBuffer[3] = (StartAddress >> 8);			//address HIGH
	IC->DataSendBuffer[4] = StartAddress;				//address LOW
	IC->DataSendBuffer[5] = WRITE_REGISTER;				//read register command
	IC->DataSendBuffer[6] = NumberOfBytes;				//number of bytes to write
	for(uint8_t i = 0; i < NumberOfBytes; i++)
	{
		IC->DataSendBuffer[7+i] = *(DataToWrite+i);
	}
	IC->DataSendBuffer[7 + NumberOfBytes] = MCP39F511_CalculateChecksum(IC, 6 + NumberOfBytes);
	MCP39F511_SendData(IC,8);								//send function
	MCP39F511_ListenDMA(IC);							//set DMA to listen
}

void MCP39F511_ParseData(MCP39F511_t *IC)
{

}
