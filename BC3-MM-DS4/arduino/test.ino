/*
  String indexOf() and lastIndexOf() functions

  Examples of how to evaluate, look for, and replace characters in a String

  created 27 Jul 2010
  modified 2 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/StringIndexOf
*/
#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial dongle(2, 3); // RX, TX

void setup()
{
    Serial.begin(115200);
    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
}

    char buf[64];
    int bytes = 0;
void loop()
{
    if (Serial.available()){
        Serial.println("user input");
        bytes = Serial.readBytes(buf,64);
        dongle.write(buf,bytes);
    }
    if (dongle.available()) {
        Serial.println("dongle input");
        bytes = dongle.readBytes(buf,64);
        Serial.write(buf,bytes);
    }
}