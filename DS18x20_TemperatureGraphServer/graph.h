

#include <stdint.h>



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
  // this value is used to scale the numbers showed in the graph
  // e.g. if the temperature of 25°C is stored as 2500
  // you can set yScale to 0.01 to display the correct axis
  float yScale; 
} graph_t;

typedef struct
{
  char * description;
  float  value;
  char * unit;
}labelText_t;
