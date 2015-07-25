/*
 ============================================================================
 Name        : ringBuffer.c
 Author      : chris
 Version     :
 Copyright   : GPL lizense 3
               ( chris (at) roboterclub-freiburg.de )
 Description : This ringBuffer overwrites circular the old values if
               the bufferSize is overflow.
               It is not a FIFO. 
 ============================================================================
 */

#include "ringBuffer.h"

void ringBufferInit( ringBuffer_t * buf, int16_t *dataArray, uint16_t bufferSize  )
{
  buf->bufferSize    = bufferSize;
  buf->data          = dataArray;
  buf->writePosition = 0;
  buf->fillSize      = 0;
}

void ringBufferAdd( ringBuffer_t * buf, int16_t value)
{
  buf->data[buf->writePosition] = value;

  buf->writePosition++;
  // wrap arround if overflow
  if ( buf->writePosition >= buf->bufferSize ) buf->writePosition = 0;  
  
  buf->fillSize++;
  // if buffer is full, no longer increase fillSize 
  if(buf->fillSize >= buf->bufferSize) buf->fillSize = buf->bufferSize;
  
}

// read a value from position n relative to the current write position
int16_t ringBufGetValue( ringBuffer_t * buf, uint16_t index)
{
  int32_t pos;

  pos = (int32_t)buf->writePosition + index - buf->fillSize;

  // if wrap arround
  if ( pos < 0 ) pos += buf->bufferSize; 

  return buf->data[pos];
}

uint16_t ringBufGetFillSize( ringBuffer_t * buf)
{
  return buf->fillSize;
}
