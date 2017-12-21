/* tobi.cpp
Created by Cherag Bhagwagar 
10/21/2016
bit |= (1<<x) sets bit x high
bit &= ~(1<<x) sets bit x low
x starts from 0

read from PCF8574AN

/////////////////////////////////////
 $ Changes to code by Andrea Frank
	marked with $ before comment.
	7/10/17
/////////////////////////////////////

*/

/***************************************************************************************/
/***************************************************************************************/
/****************************** $ GLOBAL VARIABLES *************************************/
/***************************************************************************************/
/***************************************************************************************/


// Make sure you include wire.h in your code to run this library 

#include "tobi.h"

void *__ptr1; 		// $ placeholder pointer 

#define __pcf2 57	// $ addresses for PCF IO expander
#define __pcf1 56

byte __bit1 = 0;	// $ placeholder bits
byte __bit2 = 0;


/***************************************************************************************/
/***************************************************************************************/
/************************************ $ METHODS ****************************************/
/***************************************************************************************/
/***************************************************************************************/

/////////////////////////// SETUP ////////////////////////////

/*  $         CONSTRUCTOR
	Initialize all variables on this instance.
	INPUTS: 	- None
	OUTPUTS: 	- None
	UPDATED: 	__pwmPins, __encoderPins
*/
tobi::tobi()
{
	digitalWrite(13,HIGH);		// $ turn on LED, I think?

	// for (int i = 0 ; i < 5; i ++ ){
	// 	Serial.println("Working");
	// 	__encoderVal[i] = 0;
	// }

	__pwmPins[0] = 5;
	__pwmPins[1] = 6; 		// $ changed from 5 to 12, because __encoderPins[5] = 5
	__pwmPins[2] = 9;
	__pwmPins[3] = 10;
	__pwmPins[4] = 8;
	__pwmPins[5] = 12;

	digitalWrite(13,LOW);		// $ turn off LED

	__encoderPins[0] = 0;
	__encoderPins[1] = 1;
	__encoderPins[2] = 2;
	__encoderPins[3] = 3;
	__encoderPins[4] = 4;
	__encoderPins[5] = 5;
	digitalWrite(13,HIGH); 		// $ turn on LED
}

/*  $         ENABLE
	Set up and enable hardware. Sets IO pins to proper direction, writes 0 to two pcf pins, writes 100
		to pwm pins, turns each onboard LED on, turns each onboard LED off.
	INPUTS: 	- None
	OUTPUTS: 	- None
	UPDATED:	- No variables updated, but writes 0 to pins __pcf1 and __pcf2, and 100 to all pins in __pwmPins. 
*/
void tobi::enable(void){
	Serial.println("Enabling TOBI...");
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

	//	$ The following two lines were commented out when we got the file.
	//  $	Seems it was to be used with unfinished __legUpdate method.
		//	Timer1.initialize(5000); 	//Set how frequently interrrupt occurs (5000=200Hz [DON'T CHANGE THIS!!!])
		// 	Timer1.attachInterrupt(__legUpdate_wrapper); //Set what function runs when interrupt occurs
  	for (int i = 0 ; i < 8 ; i ++){
  		tobi::led(i,1);
  		delay(100);
  	}
	for(int i = 1 ; i < 8 ; i ++) {
		tobi::led(i,0);
  		delay(100);	
	}
	Serial.println("TOBI enabled.");
}

/*  $         DISABLE
	Disable TOBI. Set all legs to speed 0, clear __bit1 and __bit2.
	INPUTS: 	- None.
	OUTPUTS: 	- None.
	UPDATED:	- __bit1, __bit2.
*/
void tobi::disable(){
	for(int i  = 0 ; i < 6 ; i ++){
		analogWrite(__pwmPins[i], 0); 	//all leg speed to 0
	}
	__bit2 = 0 ;
	__bit1 = 0 ;
	Serial.println("TOBI disabled.");
}



/////////////////////////// MOTION ////////////////////////////

/*  $         SETMOTOR
	Set motor direction. Based on our test-TOBI, motors (0,1,3,5) should be set to -1,
		and motors (2,4) should be set to 1. Uses PCF io expander and bit shifts.
	INPUTS: 	- (int) motor, (int) direction.
	OUTPUTS: 	- None
	UPDATED:	- __bit1.
*/
void tobi::setMotor(int motor , int direction){
	// $ check for valid motor number input
    if ( (motor < 0) && (motor > 5) ){
    	Serial.println("Wrong Motor");
    	return;
    }
	
	// 
    if (direction == 1)			__bit1 |= (1<<motor);        // $ assign "motor"th bit to 1    
    else if (direction == -1)	__bit1 &= ~(1<<(motor)); 	 // $ assign "motor"th bit to 0
    else Serial.println ("Invalid direction. Must choose 1 or -1.") ;
    tobi::__write8(__pcf1,__bit1);	// $ write __bit1 to __pcf1 through PCF io expander.
}

/*  $         POWERAXIS
	Turn motor axis on or off. Can only control power to motors in pairs (axes),
		M1-M2 (axis 0), M3-M4 (axis 1), and M5-M6 (axis 2).
	INPUTS: 	- (int) axis, (int) state.
	OUTPUTS: 	- None
	UPDATED:	- __bit1, __bit2.
*/
void tobi::powerAxis (int axis, int state){
	//TODO Check pins of new eagle file 
	/* turn motor axis on/off (0 -> M1-M2 ,1 -> M3-M4, 2-> M5-M6 anything else-> error
        works on enable pins for motor driver
    */
    if (axis == 0){
    	if (state == 1)			__bit1 |= (1<<6);
    	else if (state == 0)	__bit1 &= ~(1<<6);
    	tobi::__write8(__pcf1,__bit1);
    	}
    else if (axis == 1){
    	if (state == 1)			__bit1 |= (1<<7);
    	else if (state == 0)	__bit1 &= ~(1<<7);
    	tobi::__write8(__pcf1,__bit1);
    	}
    else if (axis == 2){
    	if (state == 1)			__bit2 |= (1<<0);
    	else if (state == 0)	__bit2 &= ~(1<<0);
    	tobi::__write8(__pcf2,__bit2);
    	}
   	 else Serial.println ("Wrong command") ;
}

/*  $         SETPWM
	Set PWM of motor for a leg from 0 - 255.
	INPUTS: 	- (int) leg, (int) pwm.
	OUTPUTS: 	- None.
	UPDATED:	- None.
*/
void tobi::setPwm(int leg, int pwm){
	analogWrite(__pwmPins[leg], pwm);	
}



///////////////////////// READ ENCODERS /////////////////////////

/*  $         ANALOGUPDATE
	Read encoder values into __encoderVal array for easy access. Should be called before 
		acting upon __encoderVal values.
	INPUTS: 	- None
	OUTPUTS: 	- None
	UPDATED:	- __encoderVal
*/
void tobi::analogUpdate(){
	//call in loop everytime
	__encoderVal[0] = analogRead(0);
	__encoderVal[1] = analogRead(1);
	__encoderVal[2] = analogRead(2);
	__encoderVal[3] = analogRead(3);
	__encoderVal[4] = analogRead(4);
	__encoderVal[5] = analogRead(5);
}

/*  $         GETANGLE
	Call to read the angle reported by the encoder for a specific leg. Method is necessary
		because __encoderVal is private.
	INPUTS: 	- (int) leg 	-> 	number 0 to 5 to specify which leg to query.
	OUTPUTS: 	- (int) angle 	-> 	number 0 to 730 (not sure why this range) representing angle.
	UPDATED:	- None
*/
int tobi::getAngle(int leg){
	return (__encoderVal[leg]);
}



/////////////////////////// LEDS ////////////////////////////

/*  $         LED
	Turn LED (0,1,2,3,4,5) on (1) or off (0). Note: for state, any nonzero number will be treated as on.
	INPUTS: 	- (int) led, (int) state.
	OUTPUTS: 	- None
	UPDATED:	- __bit1, __bit2. 
*/
void tobi::led(int led, int state){
	if (state == 0)		__bit2 |= (1<<led+1);
	else 				__bit2 &= ~(1<<led+1);
	tobi::__write8(__pcf2,__bit2);
}

/*  $         NOSELED
	Turn nose LED off (state = 0) or on (state = 1).
	INPUTS: 	- (int) state.
	OUTPUTS: 	- None.
	UPDATED:	- __bit2.
*/
void tobi::noseLed(int state){
	if (state == 1)		__bit2 |= (1<<7); 
	else 				__bit2 &= ~(1<<7) ;
}



/////////////////////////// HELPER METHODS ////////////////////////////

/*  $         WRITE8
	Write one byte to chosen address. Used to write to PCF expander.
	INPUTS: 	- (int) address, (byte) value.
	OUTPUTS: 	- None.
	UPDATED:	- None.
*/
void tobi::__write8(int address, byte value){
  Wire.beginTransmission(address);
  Wire.write(value);
  Wire.endTransmission();
}

/***************************************************************************************/
/***************************************************************************************/
/***************************** $ UNFINISHED METHODS ************************************/
/***************************************************************************************/
/***************************************************************************************/


/*	$ This method was left commented out by Cherag.
	$ It refers to a method "__legUpdate()" which
	$ is not included in this library. Both __legUpdate_wrapper() and __legUpdate() are commented out
	$ in the header file. However, they are called in the sample file "calibrate.ino" (this leads to
	$ an error, since we don't have the file).
*/

// void tobi::__legUpdate_wrapper(){
// 	tobi*myself = (tobi*)__ptr1;
// 	myself -> __legUpdate();
//}

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
