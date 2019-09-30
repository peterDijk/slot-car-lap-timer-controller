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

const int buttonPinCarOne = D6;
int lapsCarOne = 0;
volatile bool carOneIsPressed = false;


TM1637 tm1637(CLK,DIO);

SYSTEM_MODE(MANUAL);

void setup()
{
  tm1637.init();
  tm1637.set(0);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  tm1637.point(POINT_OFF);

  pinMode(buttonPinCarOne, INPUT_PULLUP);
  attachInterrupt(buttonPinCarOne, press, FALLING);

  Serial.begin(9600);
  Serial.println("Serial ready...");
}

void loop()
{
    if (carOneIsPressed == true)
    {
      Serial.println("Key Pressed");
      lapsCarOne++;
      carOneIsPressed = false;
    }
  ledDisplayTimer(lapsCarOne);
}

void press()
{
    //Serial.println("switched now");
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 300)  // debounce time = 50milliseconds
  {
    carOneIsPressed = true;
  }
  last_interrupt_time = interrupt_time;
}

void ledDisplayTimer(int laps) {
    if (laps == 9999) laps = 0;

    String total = String(laps);
    char CharStore[4];
    total.toCharArray(CharStore, 4);

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

    tm1637.display(0,DisplayChar0);
    tm1637.display(1,DisplayChar1);
    tm1637.display(2,DisplayChar2);
    tm1637.display(3,DisplayChar3);
}
