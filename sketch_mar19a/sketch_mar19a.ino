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
  if (temp < 15){
    type = COLD;
  }else if (temp > 37){
    type = HOT;
  }else{
    type = ROOMTEMP;
  }
  return type;
}

void displayTimeTillCooldown(float optimal) {

  // define variables
  float previousTemp = -1;
  float currentTemp = -1;
  float timeTillCool;
  float rateOfChange;

  // loop until optimal temperature is reached
  while (true) {
    if (currentTemp == -1) {
      previousTemp = getTemp();
      currentTemp = previousTemp;
      //LCD.clear();
      //LCD.print("topkek");
    } else {
      currentTemp = getTemp();
      // get rate of change of temperature per 1 second
      rateOfChange = abs((previousTemp - currentTemp)/2.5);
   
      if (!rateOfChange){
        continue;
      }
      
      timeTillCool = abs(currentTemp - optimal) / rateOfChange;

      LCD.clear();
      LCD.print("Time till cooldown:");
      LCD.setCursor(0, 1);
      LCD.print(timeTillCool);
      LCD.print(" seconds");

      if (timeTillCool < 0.1) return;
      previousTemp = currentTemp;
      delay(1500);
      
    }
  }
}
void loop() {
  
  while (digitalRead(8)){
    LCD.setRGB(255, 255, 255);
    //checkForStabilize();
    
    LCD.clear();
    LCD.print("kek");
    delay(1000);
    
    displayTimeTillCooldown(23);
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
