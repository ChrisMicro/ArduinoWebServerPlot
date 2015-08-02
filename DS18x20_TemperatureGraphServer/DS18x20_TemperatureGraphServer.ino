/*
  Web Server plotting SVG grafics

 A simple web server that shows the value of the analog input pin as a graph
 using an Arduino Wiznet Ethernet shield.
 You can simple type the IPAddress in your Browser and the plot will be
 displayed and updated every second.
 Please don't forgett to addapt the IPAdress in this program to your network.
 If you use multiple Ehternet-Shields in the same network you have als to
 modifie the MAC address.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 List of Aouthors

 18 Dec    2009  David A. Mellis    first version of the web server
  9 April  2012  Tom Igoe           modified version
 22 July   2015  ChrisMicro         SVG graphics plot implemented
 23 July   2015  ChrisMicro         multible autoscale graphs
 24 July   2015  ChrisMicro         ds18s20 temperature logger

 */

#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include "temperature.h"

#include <stdio.h>
#include "ringBuffer.h"
#include "graph.h"


//******* adapt this IPAddress to your network settings ****************

IPAddress ip(192, 168, 178, 177);

//***********************************************************************

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

#define WITDH 5
#define PREC  1

EthernetClient client;
graph_t Graph1;

#define GRAPH1_LENGTH 150
int16_t Graph1_data[GRAPH1_LENGTH]={0};
ringBuffer_t Graph1_Buffer;

/**********************************************************************************

  start SVG graphics
  
**********************************************************************************/
char strBuffer[40];

void printPar(char * str, int value)
{
    client.print(str);
    client.print("=\""); client.print(value); client.print("\" ");
}

void rect(int x,int y,int width, int height)
{
  //<rect x="10" y="10" width="100" height="100"/>
    client.print("<rect ");
    printPar("x",x);
    printPar("y",y);
    printPar("width",width);
    printPar("height",height);
    client.print("style=\"fill:ghostwhite;stroke:black;stroke-width:1;stroke-opacity:0.9\" ");
    client.println("/>");
    // <rect x="50" y="20" width="150" height="150"
    //style="fill:blue;stroke:pink;stroke-width:5;fill-opacity:0.1;stroke-opacity:0.9" />
}

void text(int x, int y, char *str, int rotation)
{
    //<text x="0" y="15" fill="red">I love SVG!</text>
    client.print("<text ");
    client.print("text-anchor=\"middle\"");
    printPar("x",x);
    printPar("y",y);
    
    // rotation
    client.print("transform=\"rotate(");
    client.print(rotation);
    client.print(" ");
    client.print(x);
    client.print(",");
    client.print(y);
    client.print(")\" ");
    
    client.print("style=\"font-size:12px\" ");
    client.print(">");
    client.print(str);
    client.println("</text>");
    //<text x="130" y="40" dx="0 0 0 -160" dy="0 0 0 -15" style="font-size:18px">
    //<text x="0" y="15" fill="red" transform="rotate(30 20,40)">I love SVG</text>
}

void labelText(labelText_t * lt)
{
  client.print(lt->description);
  dtostrf(lt->value,5,1,strBuffer);
  client.print(strBuffer);
  client.print(lt->unit);
}

#define GRAPHBORDER 40

void showGraph(graph_t * g)
{
  int n,k;
  
  float xscale=2.0;
  float yscale=1.0;
    
  int xmin;
  int xmax;
  int ymin= 32767;
  int ymax=-32767;
      
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
  
  //******** start the SVG grafics ********************************************

  client.println("<svg xmlns=\"http://www.w3.org/2000/svg\"");
  client.println("version=\"1.1\" baseProfile=\"full\"");
  //client.println("width=\"700px\" height=\"400px\" viewBox=\"0 0 700 400\">");
  client.print("width=\"");client.print(g->width+2*GRAPHBORDER);client.print("px\"");
  client.print("height=\"");client.print(g->height+2*GRAPHBORDER);client.print("px\"");
  
  //client.println("viewBox=\"0 0 700 400\" style="background: red" >");
  client.print("viewBox=\"");
  client.print(0);   client.print(" ");
  client.print(0);  client.print(" ");
  client.print(g->width+2*GRAPHBORDER);  client.print(" ");
  client.print(g->height+2*GRAPHBORDER);  client.print(" ");
  client.print("\" "); 
  client.print("style=\"background: "); 
  client.print("lightgrey");
  client.print("\" "); 
  client.println("\">");

  // graph plot area background 
  rect(GRAPHBORDER,GRAPHBORDER,g->width,g->height);
  
  //********* start of the line plot ****************
  client.println("<polyline points=\"");

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
        
    sprintf(strBuffer, "%d,%d ", x, y);
    client.print(strBuffer);
  }
  client.println("\" ");

  client.println("stroke=\"midnightblue\" fill=\"none\" stroke-width=\"1px\"/>");
  //*******  end of the line plot **********************************************

  // print title
  text(g->width/2+GRAPHBORDER,GRAPHBORDER/2,g->title,0);
  
  // print xlabel
  text(g->width/2+GRAPHBORDER,g->height + GRAPHBORDER + GRAPHBORDER/2 ,g->xlabel,0);  
  
  // print ylabel
  text(GRAPHBORDER/2,g->height/2 + GRAPHBORDER ,g->ylabel,-90);
  
  // axis xmin
  sprintf(strBuffer,"%d",g->axis[0]);
  text(GRAPHBORDER,g->height + GRAPHBORDER + GRAPHBORDER/2 ,strBuffer,0);
  
    // axis xmax
  sprintf(strBuffer,"%d",g->axis[1]);
  text(GRAPHBORDER+g->width,g->height + GRAPHBORDER + GRAPHBORDER/2 ,strBuffer,0);
  
    // axis ymin
  //sprintf(strBuffer,"%d",g->axis[2]*g->yScale);
  dtostrf(g->axis[2]*g->yScale,5,1,strBuffer);
  text(GRAPHBORDER/2,g->height + GRAPHBORDER ,strBuffer,0);
  
  // axis ymax
  //sprintf(strBuffer,"%d",g->axis[3]*g->yScale);
  dtostrf(g->axis[3]*g->yScale,5,1,strBuffer);
  text(GRAPHBORDER/2,GRAPHBORDER ,strBuffer,0);
  
  client.println("</svg>");
}

/***********************************************************************
      end SVG graphics
***********************************************************************/

float currentTemperature;
#define TEMPERATURESCALEFACTOR 100.0

void setup() 
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  ringBufferInit(&Graph1_Buffer, Graph1_data, GRAPH1_LENGTH);
  
  Graph1.ringBuffer  = &Graph1_Buffer;
  Graph1.title       = "DS18x20 Temperature Sensor";
  Graph1.xlabel      = "time";
  Graph1.ylabel      = "temperature [degree C]";
  Graph1.data        = Graph1_data;
  Graph1.data_length = GRAPH1_LENGTH;
  Graph1.width       = 420;
  Graph1.height      = 300;
  Graph1.autoscale   = true;
  Graph1.yScale      =  1/TEMPERATURESCALEFACTOR;
  // xmin xmax ymin ymax 
  Graph1.axis[0]     =    0;  
  Graph1.axis[1]     =  200;
  Graph1.axis[2]     =   25;
  Graph1.axis[3]     =   40; 
  
  initTemperatureSensor();
  currentTemperature = getTemperature();  // make sure that currentTemperature has a value

}

#define SAMPLINGTIME_MS 1000
#define SAMPLINGTIME_SEC 15*60 // every 15 minutes

//#define SAMPLINGTIME_SEC 1

void loop()
{
  int n;
  static int p=0;
  static unsigned long lastSampled = 0;
         unsigned long now         = millis();

  static int samplingTimeCounter=SAMPLINGTIME_SEC;
  
  if( now - lastSampled > 1000 ) 
  {  
    lastSampled = millis();
    
    currentTemperature = getTemperature();
    
    if( samplingTimeCounter >= SAMPLINGTIME_SEC )
    {
      ringBufferAdd(&Graph1_Buffer, currentTemperature*TEMPERATURESCALEFACTOR);
      samplingTimeCounter=0;
      Serial.println(F("nextLog"));
    }
    Serial.println(samplingTimeCounter);
    samplingTimeCounter++;
  }

  // listen for incoming clients
  client = server.available();
  if (client) {
    Serial.println(F("new client"));
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println( F("HTTP/1.1 200 OK" ) );
          client.println( F("Content-Type: text/html") );
          client.println( F("Connection: close") );  // the connection will be closed after completion of the response
          client.println( F("Refresh: 10") );  // refresh the page automatically every 5 sec

          client.println();
          client.println( F("<!DOCTYPE HTML>") );
          client.println( F("<html>") );
          client.println( F("<body bgcolor=\"#E6E6FA\">") );

          // print text message
          client.print( F("Temperature log SVG graph ") );   client.println( F("<br />") );
          client.print( F("for Arduino Uno ") );          //client.println("<br />");
          client.print( F("with Ethernet Shield") );     client.println( F("<br />") );  
          
          client.println( F("<br />") );
          
          labelText_t lt;
          lt.description="temperature:";
          lt.value=currentTemperature;
          lt.unit=" celsius";
          labelText(&lt);
          
          client.println( F("<br />") );
          client.println( F("<br />") );  
          
          client.println( F("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") );
          
          showGraph(&Graph1);
          
          client.println( F("<br />") ); 
                    
          labelText_t ult;
          ult.description = "sampling time:";
          ult.value       =  SAMPLINGTIME_SEC/60;
          ult.unit        = " minutes";
          labelText(&ult);
          
          client.println( F("<br />") ); 
          client.println( F("<form action=\"light_on.html\">") );
          client.println( F("<input type=\"submit\" value=\"Submit\">") );
          client.println( F( "</form>") );
          
          client.println( F("<form>") );
          client.println( F("First name:<br>") );
          client.println( F("<input type=\"text\" name=\"firstname\">") );
          client.println( F("<br>") );       
          client.println( F("</body></html>") ); 
          
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println(F("client disconnected"));
  }
}

