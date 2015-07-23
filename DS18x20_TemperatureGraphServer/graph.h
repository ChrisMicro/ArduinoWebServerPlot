

#include <stdint.h>

typedef struct
{
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
