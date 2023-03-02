/*
 * RingBuffer.c
 *
 *  Created on: Feb 9, 2023
 *      Author: Michal Klebokowski
 */

#include "main.h"
#include "RingBuffer.h"
#include "stm32f4xx_hal.h"

RBuffer_Status_t RBuffer_Write(RBuffer_t *Buffer,uint8_t Data)
{
	uint8_t TmpHead;
	TmpHead = (Buffer->Head + 1) % RBUFFER_SIZE;	//Modulo for looping buffer (from max to 0)
	if(TmpHead == Buffer->Tail)						//Check if buffer is full
	{
		return RBUFFER_ERROR;
	}
	Buffer->RingBuffer[Buffer->Head] = Data;
	Buffer->Head = TmpHead;
	return RBUFFER_OK;
}

RBuffer_Status_t RBuffer_Read(RBuffer_t *Buffer, uint8_t *Destination)
{
	uint8_t TmpTail;
	TmpTail = (Buffer->Tail + 1) % RBUFFER_SIZE;
	if(Buffer->Tail == Buffer->Head)
	{
		return RBUFFER_ERROR;		//no data to read
	}
	*Destination = Buffer->RingBuffer[Buffer->Tail];
	Buffer->Tail = TmpTail;
	return RBUFFER_OK;
}

void RBuffer_Flush(RBuffer_t *Buffer)
{
	Buffer->Head = 0;
	Buffer->Tail = 0;
}
