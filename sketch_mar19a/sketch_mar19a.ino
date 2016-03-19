#include <Wire.h>
#include <"rgb_lcd.h">


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  pinMode(8, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  // Check for button press
  if (digitalRead(8)) {
    
  }
  

}
