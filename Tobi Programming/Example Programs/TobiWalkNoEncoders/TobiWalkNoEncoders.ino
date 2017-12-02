#include <tobi.h>
#include <Wire.h>

//make a tobi variable
tobi t = tobi();


void setup() {
  // set data rate for serial data transfer
  Serial.begin(9600);

  //enable tobi object
  t.enable();

  // set direction for the motors so they go correct direction when powered
  t.setMotor(0,-1); // 1
  t.setMotor(1, -1); // 2
  t.setMotor(2, 1); // 3
  t.setMotor(3, -1); // 4
  t.setMotor(4, 1); // 5
  t.setMotor(5, -1); // 6

  // sets some unknown encoder field? 255 is the most accurate
  t.setPwm(0,255);
  t.setPwm(1,255);
  t.setPwm(2,255);
  t.setPwm(3,255);
  t.setPwm(4,255);
  t.setPwm(5,255);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // updates encoders?
  t.analogUpdate();

  // power axis 2 (motor 5 & 6)
  t.powerAxis(2, 1);
  // power axis 1 (motor 3 & 4)
  t.powerAxis(1, 1);
  // power axis 0 (motor 1 & 2)
  t.powerAxis(0, 1);
 
  
}
