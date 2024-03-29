/*
 * Project slot-car-lap-timer
 * Description:
 * Author: Peter van Dijk
 * Date: 29 September 2019
 */
// #include "Grove_4Digit_Display.h"
// #include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define triggerPinCarOne D6
#define triggerPinCarTwo D5
#define beeperPin A1

// TM1637 ledDisp_1(CLK_1,DIO_1);  
Adafruit_SSD1306 display(-1);
int lapsCarOne = 0;
int lapsCarTwo = 0;


/* 
 * no connect to particle cloud for local development.
 * set to semi-auto to let setup + loop run, while connecting to cloud
 */
SYSTEM_MODE(MANUAL);

void setup()
{
  // ledDisp_1.init();
  // // BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7; 
  // ledDisp_1.set(0);
  // ledDisp_1.point(POINT_ON);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (found by scanner)
  display.clearDisplay();
  
  // delay(2000);
  // display.clearDisplay();

  pinMode(beeperPin, OUTPUT);

  pinMode(triggerPinCarOne, INPUT); 
  attachInterrupt(triggerPinCarOne, triggerCarOne, FALLING);

  pinMode(triggerPinCarTwo, INPUT); 
  attachInterrupt(triggerPinCarTwo, triggerCarTwo, FALLING);

  Serial.begin(9600);
  Serial.println("Serial ready...");
}

unsigned long last_lap_moment_car_one = 0;
unsigned long last_lap_moment_car_two = 0;
unsigned long latest_laptime_car_one;
unsigned long latest_laptime_car_two;


void loop()
{
  setOledDisplay();
}

void setOledDisplay() {
  char time_buffer_1[21];
  char time_buffer_2[21];
  millis_to_laptime(latest_laptime_car_one, time_buffer_1);
  millis_to_laptime(latest_laptime_car_two, time_buffer_2);

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.print("#1 LAP:");
  display.println(lapsCarOne);
  display.println(time_buffer_1);
  display.print("#2 LAP:");
  display.println(lapsCarTwo);
  display.println(time_buffer_2);

  display.display();
}

void trigger(int carId) {
  unsigned long current_time;
  unsigned long *last_lap_moment;
  int *lapscount; // declare as pointer
  unsigned long *laptime;

  if (carId == 1) {
    last_lap_moment = &last_lap_moment_car_one; // assign address to pointer
    lapscount = &lapsCarOne;
    laptime = &latest_laptime_car_one;
  }
  if (carId == 2) {
    last_lap_moment = &last_lap_moment_car_two;
    lapscount = &lapsCarTwo; 
    laptime = &latest_laptime_car_two;
  }

  const int debounce = 300;
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > debounce) {
    // LAP
    current_time = millis();
    unsigned long _lap_time = current_time - *last_lap_moment;  
    // *pointervar = value of address stored in pointervar
    char time_buffer[21];
    millis_to_laptime(_lap_time, time_buffer);
    
    *last_lap_moment = current_time;
    *lapscount = *lapscount + 1;
    *laptime = _lap_time;

    Serial.print("Car ");
    Serial.print(carId);
    Serial.print(" LAP # ");
    Serial.print(*lapscount);
    Serial.print(" LAPTIME: ");
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

void millis_to_laptime(unsigned long millis, char *time_buffer) {
  long in_seconds = millis / 1000;
  // int runHours = in_seconds / 3600;
  int secsRemaining = in_seconds % 3600;
  int runMinutes = secsRemaining / 60;
  int runSeconds = secsRemaining % 60;
  int millisRemaining = millis % 1000;

  sprintf(time_buffer,"%02d:%02d:%02d", runMinutes, runSeconds, millisRemaining);
}