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

 */

#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>

#include <stdio.h>
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


#define GRAPH1_LENGTH 200
int16_t Graph1_data[GRAPH1_LENGTH]={0};

char strBuffer[40];

void printPar(char * str, int value)
{
    client.print(str);
    client.print("=\"");
    client.print(value);
    client.print("\" ");
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
    client.print("/>");
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
    client.print(")\"");
    
    client.print("style=\"font-size:12px\"");
    client.print(">");
    client.print(str);
    client.print("</text>");
    //<text x="130" y="40" dx="0 0 0 -160" dy="0 0 0 -15" style="font-size:18px">
    //<text x="0" y="15" fill="red" transform="rotate(30 20,40)">I love SVG</text>
}

#define GRAPHBORDER 40

void showGraph(graph_t * g)
{
  int n;
  
  float xscale=2.0;
  float yscale=1.0;
    
  int xmin;
  int xmax;
  int ymin= 32767;
  int ymax=-32767;
  
  xmin=0;
  xmax=g->data_length;
  
  if(g->autoscale)
  {
    g->axis[0] = 0;  
    g->axis[1] = g->data_length;

    for(n=0;n<g->data_length;n++)
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
  client.print(0);
  client.print(0);
  client.print(g->width);
  client.print(g->height);
  client.print("\" "); 
  client.print("style=\"background: "); 
  client.print("lightgrey");
  client.print("\" "); 
  client.println("\">");

  // graph plot area background 
  rect(GRAPHBORDER,GRAPHBORDER,g->width,g->height);
  
  //********* start of the line plot ****************
  client.println("<polyline points=\"");
  for (n = 0; n < g->data_length ; ++n)
  {
    int x,xn;
    int y,yn;
    
    xn=n;
    yn=g->data[n];
    
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
  client.print("\" ");

  client.print("stroke=\"midnightblue\" fill=\"none\" stroke-width=\"1px\"/>");
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
  sprintf(strBuffer,"%d",g->axis[2]);
  text(GRAPHBORDER/2,g->height + GRAPHBORDER ,strBuffer,0);
  
    // axis ymax
  sprintf(strBuffer,"%d",g->axis[3]);
  text(GRAPHBORDER/2,GRAPHBORDER ,strBuffer,0);
  
  client.println("</svg>");
}

/*********************************************************************** 
  OneWire DS18S20, DS18B20, DS1822 Temperature Example

   http://www.pjrc.com/teensy/td_libs_OneWire.html

   The DallasTemperature library can do all this work for you!
   http://milesburton.com/Dallas_Temperature_Control_Library

  How to connect:
  http://www.tweaking4all.com/hardware/arduino/arduino-ds18b20-temperature-sensor/
  ( Please modify the pin in the program accordingly )
  
  Example copied and modified from td_libs_OneWire.html examples
  by ChrisMicro 2015
  Copyright see "OneWire.cpp"
  
************************************************************************/

#define ONEWIRE_PIN 2

OneWire  ds(ONEWIRE_PIN);  // on pin 2 (a 4.7K resistor is necessary)

#define NUMADRESSES 8
byte addr[NUMADRESSES];
byte type_s;

//#define DEBUG_TEMPERATURESENSOR_ON

void initTemperatureSensor()
{
  byte i,n;
  //for(n=0;n<NUMADRESSES;n++)
  {
    if ( !ds.search(addr)) 
    {
        #ifdef DEBUG_TEMPERATURESENSOR_ON
      Serial.println("No more addresses.");
      Serial.println();
      #endif
      ds.reset_search();
      delay(250);
    //return;
    }
    #ifdef DEBUG_TEMPERATURESENSOR_ON
      Serial.print("ROM =");
      for( i = 0; i < 8; i++) 
      {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
      }
  
      if (OneWire::crc8(addr, 7) != addr[7]) 
      {
  
        Serial.println("CRC is not valid!");
        return;
      }
      Serial.println();
    #endif
    
    // the first ROM byte indicates which chip
    switch (addr[0]) 
    {
      case 0x10:
      {
        #ifdef DEBUG_TEMPERATURESENSOR_ON
           Serial.println("  Chip = DS18S20");  // or old DS1820
        #endif
        type_s = 1;
      }break;
        
      case 0x28:
      {
        #ifdef DEBUG_TEMPERATURESENSOR_ON
          Serial.println("  Chip = DS18B20");
        #endif
        type_s = 0;
      }break;
        
      case 0x22:
      {
        #ifdef DEBUG_TEMPERATURESENSOR_ON
          Serial.println("  Chip = DS1822");
        #endif
        type_s = 0;
      }break;
        
      default:
      {
        #ifdef DEBUG_TEMPERATURESENSOR_ON
          Serial.println("Device is not a DS18x20 family device.");
        #endif
      }break; 
    } 
  }
}

// return value: celsius_degrees
float getTemperature()
{
  byte i;
  byte present = 0;

  byte data[12];

  float celsius, fahrenheit;
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(500);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
  
  #ifdef DEBUG_TEMPERATURESENSOR_ON
    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
  #endif
  
  for ( i = 0; i < 9; i++) 
  {           // we need 9 bytes
    data[i] = ds.read();
    #ifdef DEBUG_TEMPERATURESENSOR_ON
      Serial.print(data[i], HEX);
      Serial.print(" ");
    #endif
  }
  #ifdef DEBUG_TEMPERATURESENSOR_ON
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();
  #endif

  // Convert the data to actual temperature
  // because the result is a 16 bit signed integer, it should
  // be stored to an "int16_t" type, which is always 16 bits
  // even when compiled on a 32 bit processor.
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) 
  {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // "count remain" gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else 
  {
    byte cfg = (data[4] & 0x60);
    // at lower res, the low bits are undefined, so let's zero them
    if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
    //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  #ifdef DEBUG_TEMPERATURESENSOR_ON
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");
  #endif

  return celsius;
}
/***********************************************************************
      end temperature sensor
***********************************************************************/

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
  
  Graph1.title       = "DS18x20 Temperature Sensor";
  Graph1.xlabel      = "[n]";
  Graph1.ylabel      = "temperature [degree C]";
  Graph1.data        = Graph1_data;
  Graph1.data_length = GRAPH1_LENGTH;
  Graph1.width       = 420;
  Graph1.height      = 300;
  Graph1.autoscale   = true;
  // xmin xmax ymin ymax 
  Graph1.axis[0]     =    0;  
  Graph1.axis[1]     =  200;
  Graph1.axis[2]     =  -10;
  Graph1.axis[3]     =   50; 
  initTemperatureSensor();   
}

#define SAMPLINGTIME_MS 1000

void loop()
{
  int n;
  static int p=0;
  static unsigned long lastSampled = 0;
         unsigned long now         = millis();
  
  if( now - lastSampled > SAMPLINGTIME_MS ) 
  {  
    lastSampled = millis();
    Graph1.data[p]=getTemperature();
    p++;
    if(p>GRAPH1_LENGTH-1) p=0;
  }

  // listen for incoming clients
  client = server.available();
  if (client) {
    Serial.println("new client");
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
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec

          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<body bgcolor=\"#E6E6FA\">");

          // print text message
          client.print("Multi SVG Graph Server ");   //client.println("<br />");
          client.print("for Arduino Uno");          //client.println("<br />");
          client.print("with Ethernet Shield");     client.println("<br />");  client.println("<br />");

          client.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

          //******** start the SVG grafics ********************************************
          
          showGraph(&Graph1);


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
    Serial.println("client disconnected");
  }
}

