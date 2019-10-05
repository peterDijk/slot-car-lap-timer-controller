/*
 * Project slot-car-lap-timer
 * Description:
 * Author: Peter van Dijk
 * Date: 29 September 2019
 */
#include "Grove_4Digit_Display.h"

#define CLK_1 D2
#define DIO_1 D3
#define triggerPinCarOne D6
#define triggerPinCarTwo D5


// Car 1
TM1637 ledDisp_1(CLK_1,DIO_1);
volatile bool carOneTriggered = false;
int lapsCarOne = 0;
volatile bool carTwoTriggered = false;
int lapsCarTwo = 0;


/* 
 * no connect to particle cloud for local development.
 * set to semi-auto to let setup + loop run, while connecting to cloud
 */
SYSTEM_MODE(MANUAL);

void setup()
{
  ledDisp_1.init();

  // BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7; 
  ledDisp_1.set(0);
  ledDisp_1.point(POINT_ON);

  pinMode(triggerPinCarOne, INPUT); 
  attachInterrupt(triggerPinCarOne, triggerCarOne, FALLING);

  pinMode(triggerPinCarTwo, INPUT); 
  attachInterrupt(triggerPinCarTwo, triggerCarTwo, FALLING);

  Serial.begin(9600);
  Serial.println("Serial ready...");
}

unsigned long last_lap_moment_car_one = 0;
unsigned long last_lap_moment_car_two = 0;


void loop()
{
    if (carOneTriggered) {
      lapsCarOne++;
      carOneTriggered = false;
    }
    if (carTwoTriggered) {
      lapsCarTwo++;
      carTwoTriggered = false;
    }
  ledDisplayTimer(lapsCarOne);
}

void millis_to_laptime(unsigned long millis, char *time_buffer) {
  long in_seconds = millis / 1000;
  // int runHours = in_seconds / 3600;
  int secsRemaining = in_seconds % 3600;
  int runMinutes = secsRemaining / 60;
  int runSeconds = secsRemaining % 60;
  int millisRemaining = millis % 1000;

  sprintf(time_buffer,"%02d:%02d:%02d", runMinutes, runSeconds, millisRemaining);
}

void trigger(int carId) {
  unsigned long current_time;
  unsigned long *last_lap_moment;
  volatile bool *triggered;
  int *lapscount;

  if (carId == 1) {
    last_lap_moment = &last_lap_moment_car_one;
    triggered = &carOneTriggered;
    lapscount = &lapsCarOne; 
  }
  if (carId == 2) {
    last_lap_moment = &last_lap_moment_car_two;
    triggered = &carTwoTriggered;
    lapscount = &lapsCarTwo; 
  }

  const int debounce = 300;
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > debounce) {
    // LAP
    current_time = millis();
    unsigned long lap_time = current_time - *last_lap_moment;
    
    *last_lap_moment = current_time;
    *triggered = true;

    Serial.print("Car ");
    Serial.print(carId);
    Serial.print(" LAP # ");
    Serial.print(*lapscount);
    Serial.print(" LAPTIME: ");
    char time_buffer[21];
    millis_to_laptime(lap_time, time_buffer);
    Serial.print(time_buffer);
    Serial.println(" ");
  }
  last_interrupt_time = interrupt_time;
}

void triggerCarOne() {
  if (last_lap_moment_car_one == 0) {
    last_lap_moment_car_one = millis();
    Serial.println("Car 1 has started");
  } else {
    trigger(1);
  }
}

void triggerCarTwo() {
    if (last_lap_moment_car_two == 0) {
    last_lap_moment_car_two = millis();
    Serial.println("Car 2 has started");
  } else {
    trigger(2);
  }
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

      ledDisp_1.display(3,DisplayChar3);
    } else if (totalLength == 2) {

      DisplayChar3 = DisplayChar1;
      DisplayChar2 = DisplayChar0;

      ledDisp_1.display(2,DisplayChar2);
      ledDisp_1.display(3,DisplayChar3);

    } else if (totalLength == 3) {
      DisplayChar3 = DisplayChar2;
      DisplayChar2 = DisplayChar1;
      DisplayChar1 = DisplayChar0;

      ledDisp_1.display(1,DisplayChar1);
      ledDisp_1.display(2,DisplayChar2);
      ledDisp_1.display(3,DisplayChar3);
    } else {
      ledDisp_1.display(0,DisplayChar0);
      ledDisp_1.display(1,DisplayChar1);
      ledDisp_1.display(2,DisplayChar2);
      ledDisp_1.display(3,DisplayChar3);
    }
}
