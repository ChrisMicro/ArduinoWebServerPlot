

#include <stdint.h>

typedef struct
{
  uint16_t bufferSize;
  int16_t *data;
  uint16_t fillSize;
  uint16_t writePosition;
}ringBuffer_t;

typedef struct
{
  ringBuffer_t *ringBuffer;
  int16_t *data;
  uint16_t data_length;
  uint16_t width;
  uint16_t height;
  char * title;
  char * xlabel;
  char * ylabel;
  int    axis[4]; // xmin xmax ymin ymax  
  boolean autoscale;
} graph_t;


