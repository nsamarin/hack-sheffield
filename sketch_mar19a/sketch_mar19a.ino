#include <Wire.h>
#include <Math.h>
#include "rgb_lcd.h"

rgb_lcd LCD;
const int B=4275;                 // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5

enum drinktype {COLD, ROOMTEMP, HOT};

bool buttonPressed;
bool production = false;

unsigned int red;
unsigned int green;
unsigned int blue;

unsigned int redChange;
unsigned int greenChange;
unsigned int blueChange;

float currentTemp;


void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  LCD.begin(16, 2);
}

float sampleTemp(void){
  int a = analogRead(pinTempSensor );

    float R = 1023.0/((float)a)-1.0;
    R = 100000.0*R;

    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;

    return temperature;
}

float getTemp(void){
  float sampling[1000] = {0};
  float sum = 0;
  for (int i = 0; i < 1000; i++){
    sampling[i] = sampleTemp();
    delay(1);
  }
  for (int i = 0; i < 1000; i++){
    sum += sampling[i];
  }
  return sum / 1000;
}

void checkForStabilize() {

  float temps[10] = {0};
  float minTemp;
  float maxTemp;

  //loop until temperature stabilizes
  while(true) {

    // set some unrealisitic temperatures as an edge case

    for (int i = 0; i < 10; i++) {
      temps[i] = getTemp();
//      LCD.clear();
//      LCD.print("Calibrating...");
//      LCD.setCursor(0, 1);
//      LCD.print("Temp = ");
//      LCD.print(temps[i]);
      if (i == 0) {
        minTemp = temps[i];
        maxTemp = temps[i];
      }
      else if (temps[i] < minTemp) minTemp = temps[i];
      else if (temps[i] > maxTemp) maxTemp = temps[i];
    }


    // CHECK if Temperatures are not decreasing/increasing FURTHER
    if (maxTemp - minTemp <= 1) return;

  }

}

unsigned char getDrinkType(){
  unsigned char type = 0;
  float temp = getTemp();
  if (temp < 0){
    type = COLD;
  }else if (temp > 0){
    type = HOT;
  }else{
    type = ROOMTEMP;
  }
  return type;
}

int getOptimumTemp() {
    int optimumTemp = 0;
    switch(getDrinkType()){
    case COLD:
    optimumTemp = 7;
    break;
    case ROOMTEMP:
    //roomtemp = true;
    optimumTemp = 0;
    break;
    case HOT:
    optimumTemp = 60; //was 60 by default
    break;
    }
    return optimumTemp;
}

float calculateRateOfChange(float startTemp, float temp10) {
  float k = log(abs(startTemp - 24)) / 10 - log(abs(temp10 - 24)) / 10;
  return k;
}

float calculateOptimalTime(float startTemp, float optimal, float k) {
  float t = log(abs(startTemp - 24)) / k - log(abs(optimal - 24)) / k;
  return t;
}



unsigned int calculateTime(int optimumTemp) {

  float t0 = getTemp();

  int i = 5;
  while (i) {
    if (production && !digitalRead(8)) {
      buttonPressed = false;
      return -1;
    }
    delay(2000);
    i--;
  }

  float t10 = getTemp();
  if (t0 == t10) return -1;

  
  float k = calculateRateOfChange(t0, t10);
  return round(abs(calculateOptimalTime(t0, optimumTemp, k)));
}




void loop() {


  LCD.setRGB(255, 255, 255);
  LCD.print(getTemp());
  delay(1000);
  LCD.clear();

  while (digitalRead(8)){
    buttonPressed = true;

    LCD.setRGB(255, 255, 0);
    LCD.print("Please Wait!");
    LCD.setCursor(0, 1);
    LCD.print("Calibrating...");
    checkForStabilize();

    int optimumTemp = getOptimumTemp();
    bool isRoomTemp = !optimumTemp ? true : false;
    
    while(isRoomTemp && buttonPressed) {
      float currentTemp = getTemp();
      LCD.clear();
      LCD.setRGB(0, 255, 0);
      LCD.print("Drink's good!");
      LCD.setCursor(0, 1);
      LCD.print("Temp: ");
      LCD.print(currentTemp);

      if (production && !digitalRead(8)) {
        delay(2000);
        if (!digitalRead(8)) buttonPressed = false;
      }
      
    }

    if (!buttonPressed) break;

    LCD.clear();
    LCD.setRGB(255, 255, 0);
    LCD.print("Please Wait!");
    LCD.setCursor(0, 1);
    LCD.print("Calculating...");

    // TODO! CHANGE
    unsigned int t;
    while ((t = calculateTime(25)) == -1 && buttonPressed);
    if (!buttonPressed) break;
    
    blueChange = (255 / t) ? (255 / t) : 1;
    greenChange = blueChange;
    blue = 255;
    green = 0;
    
    while (t && buttonPressed){
      currentTemp = getTemp();
      LCD.clear();
      LCD.setRGB(0, green += greenChange, blue -= blueChange);
      LCD.print("Almost Ready!");
      LCD.setCursor(0, 1);
      LCD.print("Time:");
      LCD.print(t / 60);
      LCD.print(":");
      LCD.print(t % 60);
      LCD.print("(");
      LCD.print(currentTemp);
      LCD.print(")");

      if (production && !digitalRead(8)) {
         delay(2000);
         if (!digitalRead(8)) buttonPressed = false;
      }
      
      t--;
      
    }

    
    if (!buttonPressed) break;

    LCD.clear();
    LCD.setRGB(0, 255, 0);
    LCD.print("Drink's Great!");
    LCD.setCursor(0, 1);
    LCD.print("Celsius: ");
    LCD.print(currentTemp);
    
    while ((t = calculateTime(21)) == -1 && buttonPressed);
    
    if (!buttonPressed) break;

    greenChange = (255 / t) ? (255 / t) : 1;
    redChange = greenChange;
    green = 255;
    red = 0;
    
    while (t && buttonPressed){
      currentTemp = getTemp();
      LCD.clear();
      LCD.setRGB(red += redChange, green -= greenChange, 0);
      LCD.print("Drink Faster!");
      LCD.setCursor(0, 1);
      LCD.print("Time:");
      LCD.print(t / 60);
      LCD.print(":");
      LCD.print(t % 60);
      LCD.print("(");
      LCD.print(currentTemp);
      LCD.print(")");

      if (production && !digitalRead(8)) {
        delay(2000);
        if (!digitalRead(8)) buttonPressed = false;
      }
            
      t--;
      
    }
    
    if (!buttonPressed) break;

    
    while (buttonPressed) {
      LCD.clear();
      LCD.setRGB(255, 0, 0);
      LCD.print("Oh No!");
      LCD.setCursor(0, 1);
      LCD.print("Celsius: ");
      LCD.print(currentTemp);
      currentTemp = getTemp();
      
      if (!digitalRead(8)) {
        delay(2000);
        if (!digitalRead(8)) buttonPressed = false;
      }
    }
    
  }
 
  //LCD.clear();

  
  /*
  // put your main code here, to run repeatedly:

  // Check for button press

  LCD.setRGB(255, 255, 255);
  LCD.print(getTemp());
  delay(1000);
  LCD.clear();
  */
}
