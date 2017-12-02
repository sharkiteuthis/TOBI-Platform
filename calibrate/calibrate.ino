#include <tobi.h>
#include <Wire.h>
tobi t = tobi();
int pwm = 180;
float error = 0;
void setup() {
  t.enable();
  t.powerAxis(2,0);
  t.powerAxis(1,0);
  t.powerAxis(0,1);
 t.setMotor(0,-1);
 t.setMotor(1,1);
 t.setMotor(2,-1);
 t.setMotor(3,-1);
 t.setMotor(4,-1);
 t.setMotor(5,1);
 

  t.setPwm(4,180);
  t.setPwm(5,180);
  t.setPwm(1,180);
  t.setPwm(2,180);
  t.setPwm(3,180);
  t.setPwm(0,180);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  t.legUpdate();
  error = t.getAngle(0) - t.getAngle(1);
  t.setPwm(1,pwm - error/100);
  t.setPwm(0,pwm + error/100);
  Serial.println ( error);
}
