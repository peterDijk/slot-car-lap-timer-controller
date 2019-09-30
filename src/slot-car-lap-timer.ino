/*
 * Project slot-car-lap-timer
 * Description:
 * Author: Peter van Dijk
 * Date: 29 September 2019
 */
#include "Grove_4Digit_Display.h"
// Module connection pins (Digital Pins)
#define CLK D2
#define DIO D3
#define DEBUG 1

TM1637 tm1637(CLK,DIO);

void setup()
{
  tm1637.init();
  tm1637.set(0);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.point(POINT_OFF);

  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Serial ready...");
  }
}

void loop()
{
  ledDisplayTimer();
}

void ledDisplayTimer() {
  int count = 0;

  while(true) {
    if (count == 9999) count = 0;

    String total = String(count);
    char CharStore[4];
    total.toCharArray(CharStore, 4);

    // for(unsigned char BitSelect = 0;BitSelect < 4;BitSelect ++) {
    //   ListDisp[BitSelect] = NumTab[i];
    //   i ++;
    //   if(i == sizeof(NumTab)) i = 0;
    // }

    int DisplayChar0 = CharStore[0] - '0';
    int DisplayChar1 = CharStore[1] - '0';
    int DisplayChar2 = CharStore[2] - '0';
    int DisplayChar3 = CharStore[3] - '0';

    if (CharStore[0] == 0) DisplayChar0 = 0;
    if (CharStore[1] == 0) DisplayChar1 = 0;
    if (CharStore[2] == 0) DisplayChar2 = 0;
    if (CharStore[3] == 0) DisplayChar3 = 0;

    int totalLength = total.length();
    if (totalLength == 1) {
      DisplayChar3 = DisplayChar0;
      DisplayChar0 = 0;
      DisplayChar1 = 0;
      DisplayChar2 = 0;
    }

    if (totalLength == 2) {
      DisplayChar3 = DisplayChar1;
      DisplayChar2 = DisplayChar0;
      DisplayChar1 = 0;
      DisplayChar0 = 0;
    }

    if (totalLength == 3) {
      DisplayChar3 = DisplayChar2;
      DisplayChar2 = DisplayChar1;
      DisplayChar1 = DisplayChar0;
      DisplayChar0 = 0;
    }

    if (totalLength == 4) {
    }

    if (DEBUG) {
      Serial.print(total);
      Serial.print(" --- ");
      Serial.print(DisplayChar0);
      Serial.print(" | ");
      Serial.print(DisplayChar1);
      Serial.print(" | ");
      Serial.print(DisplayChar2);
      Serial.print(" | ");
      Serial.print(DisplayChar3);
      Serial.println(" == ");
    }


    tm1637.display(0,DisplayChar0);
    tm1637.display(1,DisplayChar1);
    tm1637.display(2,DisplayChar2);
    tm1637.display(3,DisplayChar3);

    count++;
    delay(10);
  }
}
