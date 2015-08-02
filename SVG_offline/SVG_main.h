/*
 * SVG_main.h
 *
 *  Created on: 02.08.2015
 *      Author: christoph
 */

#ifndef SVG_MAIN_H_
#define SVG_MAIN_H_

#include "ringBuffer.h"
#include "SVG_graph.h"

void (*PrintTextCallBackFunction)(char * str) ;

void SVG_init(void * func);
void SVG_printText(char *str);
void printPar(char * str, int value);
char * itoa4(int i);
char * ftoa4(float f);


#endif /* SVG_MAIN_H_ */
