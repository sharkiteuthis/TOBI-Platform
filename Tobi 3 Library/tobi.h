/*   tobi library
Created by Cherag Bhagwagar
10/21/2016

/////////////////////////////////////
 $ Changes to code by Andrea Frank
	marked with $ before comment.
	7/10/17
/////////////////////////////////////

*/



#ifndef tobi_h 
#define tobi_h 
 
#include "Arduino.h"
#include <Wire.h>
#include "TimerOne.h"
#include "math.h"


class tobi
{
	
	/***************************************************************************************/
	/***************************************************************************************/
	/************************************ $ PUBLIC *****************************************/
	/***************************************************************************************/
	/***************************************************************************************/

	public:		
		/////////////////////////// SETUP ////////////////////////////

		/*  $         CONSTRUCTOR
			Initialize all variables on this instance.
			INPUTS: 	- None
			OUTPUTS: 	- None
			UPDATED: 	__pwmPins, __encoderPins
		*/
		tobi();

		/*  $         ENABLE
			Set up and enable hardware. Sets IO pins to proper direction, writes 0 to two pcf pins, writes 100
				to pwm pins, turns each onboard LED on, turns each onboard LED off.
			INPUTS: 	- None
			OUTPUTS: 	- None
			UPDATED:	- No variables updated, but writes 0 to pins __pcf1 and __pcf2, and 100 to all pins in __pwmPins. 
		*/
		void enable(void);

		/*  $         DISABLE
			Disable TOBI. Set all legs to speed 0, clear __bit1 and __bit2.
			INPUTS: 	- None.
			OUTPUTS: 	- None.
			UPDATED:	- __bit1, __bit2.
		*/
		void disable();

		/////////////////////////// MOTION ////////////////////////////

		/*  $         SETMOTOR
			Set motor direction. Based on our test-TOBI, motors (0,1,3,5) should be set to -1,
				and motors (2,4) should be set to 1. Uses PCF io expander and bit shifts.
			INPUTS: 	- (int) motor, (int) direction.
			OUTPUTS: 	- None
			UPDATED:	- __bit1.
		*/
		void setMotor(int motor , int direction);

		/*  $         POWERAXIS
			Turn motor axis on or off. Can only control power to motors in pairs (axes),
				M1-M2 (axis 0), M3-M4 (axis 1), and M5-M6 (axis 2).
			INPUTS: 	- (int) axis, (int) state.
			OUTPUTS: 	- None
			UPDATED:	- __bit1, __bit2.
		*/
		void powerAxis(int axis, int state);

		/*  $         SETPWM
			Set PWM of motor for a leg from 0 - 255.
			INPUTS: 	- (int) leg, (int) pwm.
			OUTPUTS: 	- None.
			UPDATED:	- None.
		*/
		void setPwm(int leg, int pwm);
				
		///////////////////////// READ ENCODERS /////////////////////////

		/*  $         ANALOGUPDATE
			Read encoder values into __encoderVal array for easy access. Should be called before 
				acting upon __encoderVal values.
			INPUTS: 	- None
			OUTPUTS: 	- None
			UPDATED:	- __encoderVal
		*/
		void analogUpdate(void);

		/*  $         GETANGLE
			Call to read the angle reported by the encoder for a specific leg. Method is necessary
				because __encoderVal is private.
			INPUTS: 	- (int) leg 	-> 	number 0 to 5 to specify which leg to query.
			OUTPUTS: 	- (int) angle 	-> 	number 0 to 730 (not sure why this range) representing angle.
			UPDATED:	- None
		*/
		int getAngle(int leg);

		/////////////////////////// LEDS ////////////////////////////

		/*  $         LED
			Turn LED (0,1,2,3,4,5) on (1) or off (0). Note: for state, any nonzero number will be treated as on.
			INPUTS: 	- (int) led, (int) state.
			OUTPUTS: 	- None
			UPDATED:	- __bit1, __bit2. 
		*/
		void led(int led, int state);
		
		/*  $         NOSELED
			Turn nose LED off (state = 0) or on (state = 1).
			INPUTS: 	- (int) state.
			OUTPUTS: 	- None.
			UPDATED:	- __bit2.
		*/
		void noseLed(int state);
		
		///////////////// $ UNFINISHED OR MISSING METHODS ///////////////
		void tprint(void);
		int getEncoder(int leg);
		void print (void);
		void print_raw(void);
		int angle;
		//void legUpdate(void);
		
		
	

	/***************************************************************************************/
	/***************************************************************************************/
	/*********************************** $ PRIVATE *****************************************/
	/***************************************************************************************/
	/***************************************************************************************/

	private:
		/////////////////////////// $ FIELDS ////////////////////////////
		
		// $ int __pcf1;		// $ These four are defined separately with #define or their 
		// $ int __pcf2;				// own declaration in the .cpp library so...?
		// $ byte __bit1; 
		// $ byte __bit2 ;
		int __pwmPins[6] ; 		// $ addresses of pins for writing the pwm of each motor for each leg
		int __encoderPins[6];	// $ addresses of pins for reading the value of each encoder for each leg
		int __encoderVal[6] ; 	// $ values of encoders read from each leg, updated with analogUpdate() method
		
		////////////////////////// $ METHODS ////////////////////////////
		
		/*  $         WRITE8
			Write one byte to chosen address. Used to write to PCF expander.
			INPUTS: 	- (int) address, (byte) value.
			OUTPUTS: 	- None.
			UPDATED:	- None.
		*/
		void __write8(int address, byte data);
		
		///////////////// $ UNFINISHED OR MISSING METHODS ///////////////
		void __setEncoder(void);
		//static void __legUpdate_wrapper(void);
		
};

#endif

