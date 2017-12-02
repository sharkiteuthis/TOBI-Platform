/*   tobi library
Created by Cherag Bhagwagar
10/21/2016    */

#ifndef tobi_h 
#define tobi_h 
 
#include "Arduino.h"
#include <Wire.h>
#include "TimerOne.h"
#include "math.h"


class tobi
{
	public:
		tobi();
		void enable(void);
		void setMotor(int motor , int direction );
		void powerAxis(int axis , int state);
		void tprint(void);
		int getEncoder(int leg);
		void led(int led , int state);
		void print (void);
		void print_raw(void);
		void setPwm(int leg, int pwm);
		int getAngle(int leg);
		void disable();
		int angle;
		//void legUpdate(void);
		void analogUpdate(void);
		void noseLed(int state);
		



	private:
		int __pcf1;
		int __pcf2;
		void __setEncoder(void);
		byte __bit1; 
		byte __bit2 ;
		int __pwmPins[6] ; 
		int __encoderPins[6];
		int __encoderVal[6] ; 
		void __write8(int address, byte data);
		//static void __legUpdate_wrapper(void);
		
};

#endif

