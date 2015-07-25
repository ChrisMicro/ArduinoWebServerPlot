#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __RINBBUFFER__
  #define __RINBBUFFER__
  #include <stdint.h>
  
  typedef struct
  {
    uint16_t bufferSize;
    int16_t *data;
    uint16_t fillSize;
    uint16_t writePosition;
  } ringBuffer_t;
  
  void ringBufferInit( ringBuffer_t * buf, int16_t *dataArray, uint16_t bufferSize  );
  void ringBufferAdd( ringBuffer_t * buf, int16_t value);
  int16_t ringBufGetValue( ringBuffer_t * buf, uint16_t index);
  uint16_t ringBufGetFillSize( ringBuffer_t * buf);
  #endif // __RINBBUFFER__

#ifdef __cplusplus
}
#endif
