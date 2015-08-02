/*
 ============================================================================
 Name        : HelloWorld.c
 Author      : chris
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "SVG_main.h"

FILE *SVG_file;

void Print(char * str)
{
    printf("%s",str);
    fprintf(SVG_file,"%s",str);
}

void Println(char *str)
{
	Print(str);
	Print("\n\r");
}

#define       GRAPH1_LENGTH 150
int16_t       Graph1_data[GRAPH1_LENGTH]={0};
ringBuffer_t  Graph1_Buffer;
SVG_graph_t   Graph1;

#define TEMPERATURESCALEFACTOR 100.0

void setup()
{
	ringBufferInit(&Graph1_Buffer, Graph1_data, GRAPH1_LENGTH);

	Graph1.ringBuffer  = &Graph1_Buffer;
	Graph1.title       = "DS18x20 Temperature Sensor";
	Graph1.xlabel      = "time";
	Graph1.ylabel      = "temperature [degree C]";
	Graph1.data        = Graph1_data;
	Graph1.data_length = GRAPH1_LENGTH;
	Graph1.width       = 420;
	Graph1.height      = 300;
	Graph1.autoscale   = 1;
	Graph1.yScale      =  1/TEMPERATURESCALEFACTOR;
	// xmin xmax ymin ymax
	Graph1.axis[0]     =    0;
	Graph1.axis[1]     =  200;
	Graph1.axis[2]     =   25;
	Graph1.axis[3]     =   40;
}

int main(void)
{
	int n;
	setup();
	for(n=0;n<100; n++)	ringBufferAdd(&Graph1_Buffer, n*100);

    //SVG_graphics_t svg;
    SVG_file= fopen("/home/christoph/graph.html", "w");

    SVG_init(&Print);

    Println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    Println("HTTP/1.1 200 OK");
    Println("Content-Type: text/html");
    Println("Connection: close");  // the connection will be closed after completion of the response
    Println("Refresh: 5");  // refresh the page automatically every 5 sec

    Print("");
    Println("<!DOCTYPE HTML>");
    Println("<html>");
    Println("<body bgcolor=\"#E6E6FA\">");

    // print text message
    Println("<br />");
    Print("Temperature log SVG graph ");   Println("<br />");
    Print("for Arduino Uno ");          //Println("<br />");
    Print("with Ethernet Shield");     Println("<br />");

    Println("<br />");


    labelText_t lt;
    lt.description="temperature: ";
    lt.value=25.1;
    lt.unit=" celsius";
    labelText(&lt);

    Println("<br />");
    Println("<br />");

	//SVG_test();
	showGraph(&Graph1);

	Println("<br />");

	Println("</body></html>");

	fclose(SVG_file);

	return EXIT_SUCCESS;
}





