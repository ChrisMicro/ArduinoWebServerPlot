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

#include <OneWire.h>
#include "temperature.h"

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

