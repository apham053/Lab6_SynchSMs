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
#include <stdio.h> 
#include <stdlib.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;  

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

void TimerISR() {        
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

void set_PWM(double frequency) {
  static double current_frequency;
  if (frequency != current_frequency) {
    if (!frequency) { TCCR3B &= 0x08; }
    else { TCCR3B |= 0x03; }
    if (frequency < 0.954) { OCR3A = 0xFFFF; }
    else if (frequency > 31250) { OCR3A = 0x0000; }
    else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
    TCNT3 = 0;
    current_frequency = frequency;
  }
}

void PWM_on() {
  TCCR3A = (1 << COM3A0);
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
  set_PWM(0);
}

void PWM_off() {
  TCCR3A = 0x00;
  TCCR3B = 0x00;
}

#define A0 (~PINA & 0x01)
unsigned char arrIndex = 0;
unsigned char i = 8;
double noteArr[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
enum States { start, wait, play, hold, stop} State;

void tick() {
    switch(State) {
	case start:
	    State = wait;
	    break;
	case wait:
	    i = 8;
	    if (A0) {
	        State = play;    
	    }
	    else {
		State = wait;
	    }
	    break;
	case play:
	    if (i > 0) {
		i--;
		State = play;
	    }
	    else {
		if (A0) {
		    State = hold;
		}
		else {
		    State = stop;
		}
	    }
	    break;
	case hold:
	    if (!A0) {
		State = wait;
	    }
	    else {
	        State = hold;
	    }
	    break;
	case stop:
	    State = wait;
	    break;
	default:
	    State = start;
	    break;
    }
    
    switch(State) {
	case start:
	    set_PWM(0);
	    break;
	case wait:
            break;
	case play:
	    arrIndex = (rand() % 8);
	    set_PWM(noteArr[arrIndex]);
            break;
	case hold:
            break;
	case stop:
            break;
	default:
            break;
    }
}

int main(void) {
    DDRA = 0x00;
    DDRB = 0xFF;
    PORTA = 0xFF;
    PORTB = 0x00;
    State = start;
    PWM_on();
    TimerSet(50);
    TimerOn();

    while(1){
	tick();
	while(!TimerFlag) {}
	TimerFlag = 0;
    }
 
    return 1;
}
