/*
 * SVG_main.c
 *
 *  Created on: 02.08.2015
 *      Author: christoph
 */

#include "SVG_graph.h"
#include "stdio.h"

void (*PrintTextCallBackFunction)(char * str) ;
SVG_graph_t *Graph;

char * itoa4(int i)
{
	static char buf[16];
	sprintf(buf,"%d",i);
	return buf;
}

char * ftoa4(float f)
{
	static char buf[20];
	sprintf(buf,"%.2f",f);
	return buf;
}
//dtostrf(lt->value,5,1,strBuffer);


void SVG_init(void * printOutFunction)
{
	PrintTextCallBackFunction=printOutFunction;
}

void wr(char *str)
{
	PrintTextCallBackFunction(str);
}

void wrln(char * str)
{
	wr(str);
	wr("\n\r");
}

void wrInt(int32_t value)
{
	wr(itoa4(value));
}

void printPar(char * str, int value)
{
    wr(str);
    wr("=\"");
    wrInt(value);
    wr("\" ");
}

void rect(int x,int y,int width, int height)
{
  //<rect x="10" y="10" width="100" height="100"/>
    wr("<rect ");

    printPar("x",x);
    printPar("y",y);
    printPar("width",width);
    printPar("height",height);

    wr("style=\"fill:ghostwhite;stroke:black;stroke-width:1;stroke-opacity:0.9\" ");
    wrln("/>");
    // <rect x="50" y="20" width="150" height="150"
    //style="fill:blue;stroke:pink;stroke-width:5;fill-opacity:0.1;stroke-opacity:0.9" />
}

void text(int x, int y, char *str, int rotation)
{
    //<text x="0" y="15" fill="red">I love SVG!</text>

    wr("<text ");
    wr("text-anchor=\"middle\" ");
    printPar("x",x);
    printPar("y",y);
    // rotation
    wr("transform=\"rotate(");
    wrInt(rotation);
    wr(" ");
    wrInt(x);
    wr(",");
    wrInt(y);
    wr(")\" ");

    wr("style=\"font-size:12px\" ");
    wr(">");
    wr(str);

    wrln("</text>");
    //<text x="130" y="40" dx="0 0 0 -160" dy="0 0 0 -15" style="font-size:18px">
    //<text x="0" y="15" fill="red" transform="rotate(30 20,40)">I love SVG</text>
}

void labelText(labelText_t * lt)
{
  wr(lt->description);
  wr(ftoa4(lt->value));
  wr(lt->unit);
}

#define GRAPHBORDER 40

void wrHeader()
{
	  wrln("<svg xmlns=\"http://www.w3.org/2000/svg\"");
	  wrln("version=\"1.1\" baseProfile=\"full\"");

	  wr("height=\"");wrInt(Graph->height+2*GRAPHBORDER);wr("px\" ");
	  wr("width=\"");wrInt(Graph->width+2*GRAPHBORDER);wr("px\" ");

	  wr("viewBox=\"");
	  wrInt(0);  wr(" ");
	  wrInt(0);  wr(" ");
	  wrInt(Graph->width+2*GRAPHBORDER);  wr(" ");
	  wrInt(Graph->height+2*GRAPHBORDER); wr(" ");
	  wr("\" ");
	  wr("style=\"background: "); wr("lightgrey"); wr("\" ");
	  wrln(">");
}

void wrFooter()
{
	 wrln("</svg>");
}

void showGraph(SVG_graph_t * g)
{
  int n,k;
  float f;

  float xscale=2.0;
  float yscale=1.0;

  int xmin;
  int xmax;
  int ymin= 32767;
  int ymax=-32767;

  Graph=g;

  xmin=0;
  //xmax=g->data_length;
  xmax=ringBufGetFillSize(g->ringBuffer)-1;

  if(g->autoscale)
  {
    g->axis[0] = 0;
    //g->axis[1] = g->data_length;
    g->axis[1]=ringBufGetFillSize(g->ringBuffer);

    for(n=0;n<ringBufGetFillSize(g->ringBuffer);n++)
    {
      if(g->data[n]<ymin) ymin=g->data[n];
      if(g->data[n]>ymax) ymax=g->data[n];
    }
    g->axis[2] = ymin;
    g->axis[3] = ymax;
  }else
  {
    xmin= g->axis[0];
    xmax= g->axis[1];
    ymin= g->axis[2];
    ymax= g->axis[3];
  }

  xscale=g->width/(float)(xmax-xmin);
  yscale=g->height/(float)(ymax-ymin);

  wrHeader();

  // graph plot area background
  rect(GRAPHBORDER,GRAPHBORDER,g->width,g->height);
  //*******  start of the line plot **********************************************

  wrln("<polyline points=\"");

  k=ringBufGetFillSize(g->ringBuffer);

  for (n = 0; n < k ; n++)
  {
    int x,xn;
    int y,yn;

    xn=n;
    //yn=g->data[n];
    yn=ringBufGetValue(g->ringBuffer,n);

    // limit to borders
    if( xn > xmax) xn=xmax;
    if( xn < xmin) xn=xmin;
    if( yn > ymax) yn=ymax;
    if( yn < ymin) yn=ymin;

    x =             GRAPHBORDER +  xscale * ( xn - xmin ) ;
    y = g->height + GRAPHBORDER -  yscale * ( yn - ymin ) ;

    wrInt(x);wr(",");wrInt(y);wr(" ");
  }
  wrln("\" ");

  wrln("stroke=\"midnightblue\" fill=\"none\" stroke-width=\"1px\"/>");

  //*******  end of the line plot **********************************************

  // print title
  text(g->width/2+GRAPHBORDER,GRAPHBORDER/2,g->title,0);

  // print xlabel
  text(g->width/2+GRAPHBORDER,g->height + GRAPHBORDER + GRAPHBORDER/2 ,g->xlabel,0);

  // print ylabel
  text(GRAPHBORDER/2,g->height/2 + GRAPHBORDER ,g->ylabel,-90);

  // axis xmin
  text(GRAPHBORDER,g->height + GRAPHBORDER + GRAPHBORDER/2 ,itoa4(g->axis[0]),0);

   // axis xmax
  text(GRAPHBORDER+g->width,g->height + GRAPHBORDER + GRAPHBORDER/2 ,itoa4(g->axis[1]),0);

   // axis ymin
  f=g->axis[2]*g->yScale;
  text(GRAPHBORDER/2,g->height + GRAPHBORDER ,ftoa4(f),0);

  // axis ymax
  f=g->axis[3]*g->yScale;
  text(GRAPHBORDER/2,GRAPHBORDER ,ftoa4(f),0);

  wrFooter();
}


