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

 */

#include <SPI.h>
#include <Ethernet.h>
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

void setup() {
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
}

#define WITDH 5
#define PREC  1

EthernetClient client;
graph_t Graph1;
graph_t Graph2;

#define GRAPH1_LENGTH 100
int16_t Graph1_data[GRAPH1_LENGTH];

#define GRAPH2_LENGTH 100
int16_t Graph2_data[GRAPH2_LENGTH];

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
    client.print(x);
    client.print(",");
    client.print(y);
    client.print("\"");
    
    client.print("style=\"font-size:12px\"");
    client.print(">");
    client.print(str);
    client.print("</text>");
    //<text x="130" y="40" dx="0 0 0 -160" dy="0 0 0 -15" style="font-size:18px">
    //<text x="0" y="15" fill="red" transform="rotate(30 20,40)">I love SVG</text>
}

#define GRAPHBORDER 30

void graph(graph_t * g)
{
  int n;
  char strBuffer[100];
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
    sprintf(strBuffer, "%d,%d ", n+GRAPHBORDER, g->data[n]+GRAPHBORDER);
    client.print(strBuffer);
  }
  client.print("\" ");

  client.print("stroke=\"midnightblue\" fill=\"none\" stroke-width=\"1px\"/>");
  //*******  end of the line plot **********************************************

  // print title
  text(g->width/2+GRAPHBORDER,GRAPHBORDER/2,g->title,0);
  
  // print xlabel
  text(g->width/2+GRAPHBORDER,g->height + GRAPHBORDER + GRAPHBORDER/2 ,g->xlabel,20);  
  
  client.println("</svg>");
}


void loop()
{
  char strBuffer[40];
  int n;
  
  Graph1.title       = "Analog Channel 0";
  Graph1.xlabel      = "[n]";
  Graph1.ylabel      = "[v]";
  Graph1.data        = Graph1_data;
  Graph1.data_length = GRAPH1_LENGTH;
  Graph1.width       = 200;
  Graph1.height      = 150;

  for (n = 0; n < GRAPH1_LENGTH ; ++n)
  {
    Graph1_data[n]= analogRead(A0);
  }
  
  Graph2.title       = "Analog Channel 1";
  Graph2.xlabel      = "[n]";
  Graph2.ylabel      = "[v]";
  Graph2.data        = Graph2_data;
  Graph2.data_length = GRAPH2_LENGTH;
  Graph2.width       = 300;
  Graph2.height      = 200;

  for (n = 0; n < GRAPH2_LENGTH ; ++n)
  {
    Graph2_data[n]= analogRead(A1);
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

          // print text message
          client.print("analog channel 0 over time ");

          client.println("<br />");

          client.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

          //******** start the SVG grafics ********************************************
          
          graph(&Graph1);
          graph(&Graph2);

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

