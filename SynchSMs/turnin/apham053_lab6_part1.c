/*	Author: apham053
 *  Partner(s) Name: Steven Rodriguez
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
   
volatile unsigned char TimerFlag = 0;  //

unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 

void TimerOn() {
  TCCR1B = 0x0B;
  OCR1A = 125;
  TIMSK1 = 0x02;
  TCNT1 = 0;
  _avr_timer_cntcurr = _avr_timer_M;
  SREG |= 0x80;
}

void TimerOff() {
  TCCR1B = 0x00;
}

void TimerISR() {        //
  TimerFlag = 1;
}  

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
 

    enum States { Start, blink1, blink2, blink3 } State;
    unsigned char C = 0x00; 
    
    void tick() {
	switch(State) {
	    case Start:
		C = 0x00;
		State = blink1;
		break;
	    case blink1:
		C = 0x01; 
		State = blink2;
		break;
	    case blink2:
		C = 0x02;
		State = blink3;
		break;
	    case blink3: 
		C = 0x04;
		State = blink1;
		break;
	    default: 
		State = Start;
		break;
   	}

	switch(State) {
	    case Start:
		break;
	    case blink1:
                break;
	    case blink2:
                break;
	    case blink3:
                break;
	    default:
		break;
	}
	
	PORTC = C;
    }

int main (void) {
    DDRC = 0xFF;
    PORTC = 0x00;
    C = 0x00; 
    State = Start;
    TimerSet(1000); 
    TimerOn();	    
    
    while(1) {
	tick();
	while(!TimerFlag) {}
	    TimerFlag = 0;	
    }	   
    return 1;
}
