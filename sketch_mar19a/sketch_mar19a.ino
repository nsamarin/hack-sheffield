#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd LCD;
const int B=4275;                 // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5
 

void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  LCD.begin(16, 2); 
}

float gettemp(void){
  int a = analogRead(pinTempSensor );
 
    float R = 1023.0/((float)a)-1.0;
    R = 100000.0*R;
 
    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;

    return temperature;
}


void loop() {
  // put your main code here, to run repeatedly:

  // Check for button press
  if (digitalRead(8)) {
    LCD.setRGB(255, 255, 255);
    LCD.print("Temperature = ");
    LCD.print(gettemp());
    delay(1000);
  }
  LCD.clear();
}
