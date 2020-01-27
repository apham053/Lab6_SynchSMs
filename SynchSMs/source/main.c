/*	Author: apham053
 *  Partner(s) Name: Steven Rodriguez
 *	Lab Section:
 *	Assignment: Lab #6  Exercise #3
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
 
unsigned char C = 0x07;        
#define A0 (PINA & 0x01)  
#define A1 (PINA & 0x02)  

enum States { Start, Wait, Add, Sub, Reset, Release} State;

void tick() {
    switch(State) {
        case Start:
	    C = 0x07;       
	    State = Wait; 	
	    break;
	case Wait: 
		if (A0 && !A1) {
			State = Add;
		}
		else if (!A0 && A1) {
			State = Sub;
		}
		else if (A0 && A1) {
			State = Reset;
		}
		else {
			State = Wait;
		}
	    break;
	case Add:   
		State = Release;
	    break;
	case Sub:
		State = Release;
            break;
	case Reset:
		State = Release;
	    break;
	case Release:
		if (!A0 && !A1) {
			State = Wait;
		}
		else if (A0 && A1) {
			State = Reset;
		}
		else { 
			State = Release;
		}
	        break;
    	default:
		State = Start;
		break;
    }
    
    switch(State) {
	case Start:
	    break;
	case Wait:
	    break;
	case Add:
	    if (C < 0x09) {
                C = C + 1;
            }
	    else {
                C = 0x09;
            }
            break;
	case Sub:
	    if (C > 0x00) {
		C = C - 1;
	    }
	    else {
		C = 0x00;
	    }
            break;
	case Reset:
	    C = 0x00;
            break;
    case Release:
		break;
	default:
	    break;	    
    }    
}   

int main (void) {
    DDRA = 0x00;
    DDRC = 0xFF;
    PORTA = 0xFF;
    PORTC = 0x00;
    C = 0x00; 
    State = Start;
    TimerSet(100); 
    TimerOn();	    
    
    while(1) {
	tick();
	while(!TimerFlag) {}
	    TimerFlag = 0;	
    }	   
    return 1;
}
