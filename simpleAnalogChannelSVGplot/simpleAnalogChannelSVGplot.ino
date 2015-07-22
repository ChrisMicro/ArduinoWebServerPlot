/*
  Web Server plotting the analog channel 0

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

void loop()
{
  char strBuffer[100];
  int n;

  // listen for incoming clients
  EthernetClient client = server.available();
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
          client.println("Refresh: 1");  // refresh the page automatically every 5 sec
          
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          
          // print text message
          client.print("analog channel 0 over time ");
          client.println("<br />");

          //******** start the SVG grafics ********************************************
          client.println("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
          client.println("<svg xmlns=\"http://www.w3.org/2000/svg\"");
          client.println("version=\"1.1\" baseProfile=\"full\"");
          client.println("width=\"700px\" height=\"400px\" viewBox=\"0 0 700 400\">");
          
          //********* start of the line plot ****************
          client.println("<polyline points=\"");
          for (n = 0; n < 700 ; ++n)
          {
            sprintf(strBuffer, "%d,%d ", n, analogRead(A0));
            client.print(strBuffer);

          }
          client.print("\" ");

          client.print("stroke=\"green\" fill=\"none\" stroke-width=\"1px\"/>");
          //*******  end of the line plot **********************************************
          
          client.println("</svg>");

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

