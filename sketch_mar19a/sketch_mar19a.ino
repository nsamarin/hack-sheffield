#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd LCD;

void setup() {
  // put your setup code here, to run once:
  pinMode(8, INPUT);
  LCD.begin(16, 2); 
}

  


void loop() {
  // put your main code here, to run repeatedly:

  // Check for button press
  if (digitalRead(8)) {
    LCD.setRGB(255, 255, 255);
    delay(1000);
    LCD.print("Hello, World!");
  }else{
    LCD.setRGB(0, 0, 0); 
  }
  

}
