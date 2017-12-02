/* tobi.cpp
Created by Cherag Bhagwagar 
10/21/2016
bit |= (1<<x) sets bit x high
bit &= ~(1<<x) sets bit x low
x starts from 0

read from PCF8574AN
*/


// Make sure you include wire.h in your code to run this library 

#include "tobi.h"

void *__ptr1;

#define __pcf2 57
#define __pcf1 56

byte __bit1 = 0;
byte __bit2 = 0;

//Initialize all variables on constructor
tobi::tobi()
{
	digitalWrite(13,HIGH);

	// for (int i = 0 ; i < 5; i ++ ){
	// 	Serial.println("Working");
	// 	__encoderVal[i] = 0;
	// }

	__pwmPins[0] = 9;
	__pwmPins[1] = 5;
	__pwmPins[2] = 6;
	__pwmPins[3] = 10;
	__pwmPins[4] = 11;
	__pwmPins[5] = 13;

	digitalWrite(13,LOW);

	__encoderPins[0] = 0;
	__encoderPins[1] = 1;
	__encoderPins[2] = 2;
	__encoderPins[3] = 3;
	__encoderPins[4] = 4;
	__encoderPins[5] = 5;
	digitalWrite(13,HIGH);
}


// void tobi::__legUpdate_wrapper(){
// 	tobi*myself = (tobi*)__ptr1;
// 	myself -> __legUpdate();
//}

void tobi::analogUpdate(){
	//call in loop everytime
	__encoderVal[0] = analogRead(0);
	__encoderVal[1] = analogRead(1);
	__encoderVal[2] = analogRead(2);
	__encoderVal[3] = analogRead(3);
	__encoderVal[4] = analogRead(4);
	__encoderVal[5] = analogRead(5);
}


int tobi::getAngle(int leg){
	return (__encoderVal[leg]);
}

void tobi::enable(void){
	Serial.println("enable tobi");
	/* Sets the robot in Tobi Mode, enabling and disabling pins */

	//Set PWM pins to output 
	for (int i = 0 ; i < 6 ; i ++){
		pinMode(__pwmPins[i],OUTPUT);
	}

	//Set encoder pins to input
	for (int i = 0 ; i < 6 ; i ++){
		pinMode(__encoderPins[i],INPUT);
	}

	Wire.begin();
	tobi::__write8(__pcf1,0);
	tobi::__write8(__pcf2,0);

	for(int i = 0 ; i < 6 ; i ++) {
	analogWrite(__pwmPins[i],100);	
	}

//	Timer1.initialize(5000);//Set how frequently interrrupt occurs (5000=200Hz [DON'T CHANGE THIS!!!])
 // 	Timer1.attachInterrupt(__legUpdate_wrapper); //Set what function runs when interrupt occurs
  	for (int i = 0 ; i < 8 ; i ++){
  		tobi::led(i,1);
  		delay(100);
  	}
	for(int i = 1 ; i < 8 ; i ++) {
		tobi::led(i,0);
  		delay(100);	
	}
}

void tobi::led(int led, int state){
	//turn LED 0,1,2,3,4,5 on/off. 1-> on 0-> off

	if (state == 0){
		__bit2 |= (1<<led+1);
	}
	else {
		 __bit2 &= ~(1<<led+1);
	}
	tobi::__write8(__pcf2,__bit2);
}


void tobi::print_raw(){
	//TODO
	Serial.print("BIT 1:\t");
	Serial.println(__bit1,BIN);
	Serial.print("BIT 2:\t");
	Serial.println(__bit2,BIN);
	Serial.println("---------------------------");

	}

void tobi::print(){
	//prints out all angle in format 
	// angle leg <leg #> <angle value>
	for (int i = 0 ; i < 6; i ++){
		Serial.print("angle leg "); Serial.print(i); Serial.print("  ");Serial.println(__encoderVal[i]);
	}

	//TODO
	}

void tobi::setMotor(int motor , int direction){
	/* motors range from 0-5
        direction 1 -> front  -1 -> back else -> nothing
        uses PCF io expander and bit shifts 
        
    */
    if ( (motor < 0) && (motor > 5) ){
    	Serial.println("Wrong Motor");
    	return  ;
    }
    if (direction == 1)			__bit1 |= (1<<motor);            
    else if (direction == -1)	__bit1 &= ~(1<<(motor));
    else Serial.println ("Wrong command") ;
    tobi::__write8(__pcf1,__bit1);
}

void tobi::powerAxis (int axis, int state){
	//TODO Check pins of new eagle file 
	/* turn motor axis on/off (0 -> M1-M2 ,1 -> M3-M4, 2-> M5-M6 anything else-> error
        works on enable pins for motor driver
    */
    if (axis == 0){
    	if (state == 1)		__bit1 |= (1<<6);
    	else if (state == 0)	__bit1 &= ~(1<<6);
    	tobi::__write8(__pcf1,__bit1);
    	}
    else if (axis == 1){
    	if (state == 1)		__bit1 |= (1<<7);
    	else if (state == 0)	__bit1 &= ~(1<<7);
    	tobi::__write8(__pcf1,__bit1);
    	}
    else if (axis == 2){
    	if (state == 1)		__bit2 |= (1<<0);
    	else if (state == 0)	__bit2 &= ~(1<<0);
    	tobi::__write8(__pcf2,__bit2);
    	}
   	 else Serial.println ("Wrong command") ;
   

	}

void tobi::__write8(int address, byte value){
  Wire.beginTransmission(address);
  Wire.write(value);
  Wire.endTransmission();
}

void tobi::setPwm(int leg, int pwm){
	/* give pwm ranging from 0 - 255 on legs 0 1 2 3 4 5
	    */
	analogWrite(__pwmPins[leg],pwm);
}

void tobi::disable(){
	for(int i  = 0 ; i < 6 ; i ++){
		analogWrite(__pwmPins[i],0); 	//all leg speed to 0
	}
	__bit2 = 0 ;
	__bit1 = 0 ;
	Serial.println("Tobi disable");
}

void tobi::noseLed(int state){
	if (state == 1)	__bit2 |= (1<<7); 
	else __bit2 &= ~(1<<7) ;
}
