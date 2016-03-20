#include <Wire.h>
#include <Math.h>
#include "rgb_lcd.h"

rgb_lcd LCD;
const int B=4275;                 // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5

enum drinktype {COLD, ROOMTEMP, HOT};


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
      LCD.clear();
      LCD.print("Calibrating...");
      LCD.setCursor(0, 1);
      LCD.print("Temp = ");
      LCD.print(temps[i]);
      if (i == 0) {
        minTemp = temps[i];
        maxTemp = temps[i];
      }
      else if (temps[i] < minTemp) minTemp = temps[i];
      else if (temps[i] > maxTemp) maxTemp = temps[i];
    }

    if (maxTemp - minTemp <= 1) return;

  }

}

unsigned char getDrinkType(){
  unsigned char type = 0;
  float temp = getTemp();
  if (temp < 7){
    type = COLD;
  }else if (temp > 60){
    type = HOT;
  }else{
    type = ROOMTEMP;
  }
  return type;
}

float calculateRateOfChange(float startTemp, float temp10) {
  float k = log(abs(startTemp - 26)) / 10 - log(abs(temp10 - 26)) / 10;
  return k;
}

float calculateOptimalTime(float startTemp, float optimal, float k) {
  float t = log(abs(startTemp - 26)) / k - log(abs(optimal - 26)) / k;
  return t;
}



unsigned int calculateTime(unsigned char optimumTemp) {

  float t0 = getTemp();
  delay(10000);
  float t10 = getTemp();
  float k = calculateRateOfChange(t0, t10);
  return round(abs(calculateOptimalTime(t0, optimumTemp, k)));
}

void loop() {

  while (digitalRead(8)){
    LCD.setRGB(255, 255, 255);
    //checkForStabilize();
    LCD.clear();
    LCD.print("Calculating time...");
    

    unsigned char optimumTemp = 0;
    bool roomtemp = false;
    
    switch(getDrinkType()){
    case COLD:
    optimumTemp = 7;
    break;
    case ROOMTEMP:
    //roomtemp = true;
    optimumTemp = 25;
    break;
    case HOT:
    optimumTemp = 60;
    break;
    }
    unsigned int t = calculateTime(optimumTemp);
    
    while (t && roomtemp == false){
      float temp = getTemp();
      LCD.clear();
      LCD.print("You have:");
      LCD.setCursor(0, 1);
      LCD.print(t);
      LCD.print(" Temp = ");
      LCD.print(temp);
      t--;
    }
  }
  if(digitalRead(8)){
  LCD.clear();
  LCD.print("Your drink has");
  LCD.setCursor(0, 1);
  LCD.print("cooled down");
  }
  
  for(;digitalRead(8) == 1;){
    delay(10);
  }
  
  LCD.clear();
  /*
  // put your main code here, to run repeatedly:

  // Check for button press

  LCD.setRGB(255, 255, 255);
  LCD.print(getTemp());
  delay(1000);
  LCD.clear();
  */
}



// void displayTimeTillCooldown(float optimal) {
//
//   // define variables
//   float previousTemp = -1;
//   float currentTemp = -1;
//   float timeTillCool;
//   float rateOfChange;
//
//   // loop until optimal temperature is reached
//   while (true) {
//     if (currentTemp == -1) {
//       previousTemp = getTemp();
//       currentTemp = previousTemp;
//       //LCD.clear();
//       //LCD.print("topkek");
//     } else {
//       currentTemp = getTemp();
//       // get rate of change of temperature per 1 second
//       rateOfChange = abs((previousTemp - currentTemp)/11);
//
//       if (!rateOfChange){
//         continue;
//       }
//
//       timeTillCool = abs(currentTemp - optimal) / rateOfChange;
//
//       LCD.clear();
//       LCD.print("Cooldown in:");
//       LCD.setCursor(0, 1);
//       LCD.print(timeTillCool);
//       LCD.print("s ");
//       LCD.print(currentTemp);
//       LCD.print("/");
//       LCD.print(optimal);
//
//
//       if (timeTillCool < 0.1) return;
//       previousTemp = currentTemp;
//       delay(10000);
//
//     }
//   }
// }
