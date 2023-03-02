/*
 * RingBuffer.h
 *
 *  Created on: Feb 9, 2023
 *      Author: Michal Klebokowski
 */

#ifndef INC_RINGBUFFER_H_
#define INC_RINGBUFFER_H_

#define RBUFFER_SIZE 32

typedef enum
{
  RBUFFER_OK       = 0x00U,
  RBUFFER_ERROR    = 0x01U,
} RBuffer_Status_t;

typedef struct{
	uint8_t RingBuffer[RBUFFER_SIZE];
	uint8_t Head;
	uint8_t Tail;
}RBuffer_t;

RBuffer_Status_t RBuffer_Write(RBuffer_t *Buffer,uint8_t Data);
RBuffer_Status_t RBuffer_Read(RBuffer_t *Buffer, uint8_t *Destination);
void RBuffer_Flush(RBuffer_t *Buffer);


#endif /* INC_RINGBUFFER_H_ */
