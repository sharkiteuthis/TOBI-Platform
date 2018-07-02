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
#include "SoftPWM.h"

//void *__ptr1; 		// $ placeholder pointer 

#define __pcf3 61
#define __pcf2 57	// $ addresses for PCF IO expander
#define __pcf1 56

byte __byte1 = 0;	// $ placeholder bits
byte __byte2 = 0;
byte __byte3 = 0;

#define PWM_PIN0 5
#define PWM_PIN1 6
#define PWM_PIN2 9
#define PWM_PIN3 10

//These are the pins for the third axis, but the SoftPWM library is managing them
SOFTPWM_DEFINE_CHANNEL(0, DDRD, PORTD, PORTD4)  // pin 4
SOFTPWM_DEFINE_CHANNEL(1, DDRD, PORTD, PORTD6)	// pin 12
SOFTPWM_DEFINE_OBJECT(2);


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
	UPDATED: 	__encoderPins
*/
tobi::tobi()
{
	// set analog pins for encoders
	__encoderPins[0] = 0;
	__encoderPins[1] = 1;
	__encoderPins[2] = 2;
	__encoderPins[3] = 3;
	__encoderPins[4] = 4;
	__encoderPins[5] = 5;

	//turn on LED
	digitalWrite(13,HIGH);
}

/*  $         ENABLE
	Set up and enable hardware. Sets IO pins to proper direction, writes 0 to two pcf pins, writes 100
		to pwm pins, turns each onboard LED on, turns each onboard LED off.
	INPUTS: 	- None
	OUTPUTS: 	- None
	UPDATED:	- No variables updated, but writes 0 to all port expanders. 
*/
void tobi::enable(void){
	Serial.println("Enabling TOBI...");
	/* Sets the robot in Tobi Mode, enabling and disabling pins */

	// initialize software pwm at 100hz and set leg speed to zero
	Palatis::SoftPWM.begin(100);
	Palatis::SoftPWM.set(0,0);
	Palatis::SoftPWM.set(1,0);

	//Set hardware PWM pins to output 
	pinMode(PWM_PIN0, OUTPUT);
	pinMode(PWM_PIN1, OUTPUT);
	pinMode(PWM_PIN2, OUTPUT);
	pinMode(PWM_PIN3, OUTPUT);

	//Set encoder pins to input
	for (int i = 0 ; i < 6 ; i ++){
		pinMode(__encoderPins[i],INPUT);
	}

	Wire.begin();
	
	tobi::__write8(__pcf1,0);
	tobi::__write8(__pcf2,0);
	tobi::__write8(__pcf3,0);

	//	$ The following two lines were commented out when we got the file.
	//  $	Seems it was to be used with unfinished __legUpdate method.
		//	Timer1.initialize(5000); 	//Set how frequently interrrupt occurs (5000=200Hz [DON'T CHANGE THIS!!!])
		// 	Timer1.attachInterrupt(__legUpdate_wrapper); //Set what function runs when interrupt occurs

	// blink all of the LEDs twice each in sequence
  	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 6; i++) {
  			tobi::led(i,1);
  			delay(500);
  		}
		noseLed(1);
		delay(500);
		noseLed(0);
	}
	
	Serial.println("TOBI enabled.");
}

/*  $         DISABLE
	Disable TOBI. Set all legs to speed 0, clear pcf bytes
	INPUTS: 	- None.
	OUTPUTS: 	- None.
	UPDATED:	- __byte1, __byte2, __byte3
*/
void tobi::disable(){
	//turn off the legs
	analogWrite(PWM_PIN0, 0);
	analogWrite(PWM_PIN1, 0);
	analogWrite(PWM_PIN2, 0);
	analogWrite(PWM_PIN3, 0);
	Palatis::SoftPWM.set(0,0);
	Palatis::SoftPWM.set(1,0);
	
	__byte3 = 0;
	__byte2 = 0;
	__byte1 = 0;
	
	tobi::__write8(__pcf1,0);
	tobi::__write8(__pcf2,0);
	tobi::__write8(__pcf3,0);
	
	Serial.println("TOBI disabled.");
}



/////////////////////////// MOTION ////////////////////////////

/*  $         SETMOTOR
	Set motor direction. Based on our test-TOBI, motors (1,3,5) should be set to -1,
		and motors (0,2,4) should be set to 1. Uses PCF io expander and bit shifts.
	INPUTS: 	- (int) motor, (int) direction.
	OUTPUTS: 	- None
	UPDATED:	- __byte1.
*/
void tobi::setMotor(int motor , int direction){
	
	byte __write_port = 0;			//ports are #defines, so no need for a ptr here
	byte * __write_byte = NULL;
	byte shift = motor % 2 ? 2 : 0;
	
	switch(motor) {
		case 0:
		case 1:
			__write_port = __pcf1;
			__write_byte = &__byte1;
			break;
		case 2:
		case 3:
			__write_port = __pcf2;
			__write_byte = &__byte2;
			break;
		case 4:
		case 5:
			__write_port = __pcf3;
			__write_byte = &__byte3;
			break;
		default:
			Serial.println("Leg number not in range");
	}
	
	//sanity check
	if(__write_byte == 0 || __write_port == NULL) {
		Serial.println("Unknown error setting motor direction");
		return;
	}
	
	//use bitwise ops and the `shift` variable to set motor direction
    if (direction == 1){
		    *__write_byte |= 1<<(shift+1);
			*__write_byte &= ~(1<<shift);
	} else if (direction == -1) {
		    *__write_byte |= 1<<shift;
			*__write_byte &= ~(1<<(shift+1));
	} else {
		Serial.println("Invalid direction. Must choose 1 or -1.");
	}
		
    tobi::__write8(__write_port, &__write_byte);
}

/*  $         POWERAXIS
	Turn motor axis on or off. Can only control power to motors in pairs (axes),
		M1-M2 (axis 0), M3-M4 (axis 1), and M5-M6 (axis 2).
	INPUTS: 	- (int) axis, (int) state.
	OUTPUTS: 	- None
	UPDATED:	- __byte1, __byte2, __byte3.
*/
void tobi::powerAxis (int axis, int state){
	//TODO Check pins of new eagle file 
	/* turn motor axis on/off (0 -> M1-M2 ,1 -> M3-M4, 2-> M5-M6 anything else-> error
        works on enable pins for motor driver
    */
    if (axis == 0){
    	if (state == 1)			__byte1 |= (1<<4);
    	else if (state == 0)	__byte1 &= ~(1<<4);
    	tobi::__write8(__pcf1,__byte1);
    	}
    else if (axis == 1){
    	if (state == 1)			__byte1 |= (1<<4);
    	else if (state == 0)	__byte1 &= ~(1<<4);
    	tobi::__write8(__pcf1,__byte1);
    	}
    else if (axis == 2){
    	if (state == 1)			__byte3 |= (1<<4);
    	else if (state == 0)	__byte3 &= ~(1<<4);
    	tobi::__write8(__pcf3,__byte3);
    	}
   	 else Serial.println("Wrong command") ;
}

/*  $         SETPWM
	Set PWM of motor for a leg from 0 - 255.
	INPUTS: 	- (int) leg, (int) pwm.
	OUTPUTS: 	- None.
	UPDATED:	- None.
*/
void tobi::setPwm(int leg, int pwm){
	if(pwm > 255 || pwm < 0) {
		Serial.println("PWM out of range");
		return;
	}
	
	switch(leg) {
		case 0:
			analogWrite(PWM_PIN0, pwm);
			break;
		case 1:
			analogWrite(PWM_PIN1, pwm);
			break;
		case 2:
			analogWrite(PWM_PIN2, pwm);
			break;
		case 3:
			analogWrite(PWM_PIN3, pwm);
			break;
		case 4:
			Palatis::SoftPWM.set(0,pwm);
			break;
		case 5:
			Palatis::SoftPWM.set(1,pwm);
			break;
		default:
			Serial.println("Leg number not in range");
	}
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

#define CHANGE_LED_STATE(pcf, byte, pin, state) \
	if((state) == 0) byte |= (1<<(pin)); \
	else			 byte &= ~(1<<(pin)); \
	tobi::__write8((pcf),(byte));

/*  $         LED
	Turn LED (0,1,2,3,4,5) on (1) or off (0). Note: for state, any nonzero number will be treated as on.
	INPUTS: 	- (int) led, (int) state.
	OUTPUTS: 	- None
	UPDATED:	- __byte1, __byte2, __byte3. 
*/
void tobi::led(int led, int state){
	if(state != 0 && state != 1) {
		Serial.println("LED state must be 0 or 1");
		return;
	}

	switch(led) {
		case 0:
			CHANGE_LED_STATE(__pcf1, __byte1, 5, state);
			break;
		case 1:
			CHANGE_LED_STATE(__pcf1, __byte1, 6, state);
			break;
		case 2:
			CHANGE_LED_STATE(__pcf2, __byte2, 5, state);
			break;
		case 3:
			CHANGE_LED_STATE(__pcf2, __byte2, 6, state);
			break;
		case 4:
			CHANGE_LED_STATE(__pcf3, __byte3, 5, state);
			break;
		case 5:
			CHANGE_LED_STATE(__pcf3, __byte3, 6, state);
			break;
		default:
			Serial.println("LED out of range");
	}
}

/*  $         NOSELED
	Turn nose LED off (state = 0) or on (state = 1).
	INPUTS: 	- (int) state.
	OUTPUTS: 	- None.
	UPDATED:	- __byte2.
*/
void tobi::noseLed(int state){
	if(state != 0 && state != 1) {
		Serial.println("LED state must be 0 or 1");
		return;
	}
	
	CHANGE_LED_STATE(__pcf1, __byte1, 7, state);
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

void print_bits(byte myByte){
 for(byte mask = 0x80; mask; mask >>= 1){
   if(mask  & myByte)
       Serial.print('1');
   else
       Serial.print('0');
 }
}

void tobi::print_raw(){

	Serial.print("BIT 1:\t");
	print_bits(__byte1);
	Serial.println();
	
	Serial.print("BIT 2:\t");
	print_bits(__byte2);
	Serial.println();
	
	Serial.print("BIT 3:\t");
	print_bits(__byte3);
	Serial.println();
	
	Palatis::SoftPWM.printInterruptLoad();
	
	Serial.println("---------------------------");
	Serial.println();
}

void tobi::print(){
	//prints out all angle in format 
	// angle leg <leg #> <angle value>
	for (int i = 0 ; i < 6; i ++){
		Serial.print("angle leg "); Serial.print(i); Serial.print("  ");Serial.println(__encoderVal[i]);
	}

	//TODO
}
