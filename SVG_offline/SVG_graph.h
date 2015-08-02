/*
 * SVG_graph.h
 *
 *  Created on: 02.08.2015
 *      Author: christoph
 */

#ifndef SVG_GRAPH_H_
#define SVG_GRAPH_H_

#include <stdint.h>
#include "ringBuffer.h"

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
  uint8_t  autoscale;
  // this value is used to scale the numbers showed in the graph
  // e.g. if the temperature of 25°C is stored as 2500
  // you can set yScale to 0.01 to display the correct axis
  float yScale;
} SVG_graph_t;

typedef struct
{
  char * description;
  float  value;
  char * unit;
}labelText_t;

void showGraph(SVG_graph_t * g);
void labelText(labelText_t * lt);

#endif /* SVG_GRAPH_H_ */
