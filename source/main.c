/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #3
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

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;

        OCR1A = 125;

        TIMSK1 = 0X02;

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
        if(_avr_timer_cntcurr == 0) {
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
        if(!frequency) {
            TCCR3B &= 0x08;
	}
        else {
            TCCR3B |= 0x03;
	}

        if(frequency < 0.954) {
            OCR3A = 0xFFFF;
	}
        else if (frequency > 31250) {
            OCR3A = 0x0000;
	}
        else {
            OCR3A = (short) (8000000 / (128 * frequency)) - 1;
	} 
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

enum STATES {start, checkButton, playSong, timeBetweenNotes, } STATE;

unsigned char temp = 0x00;

void playMelody() {
        temp = ~PINA;
	unsigned int i = 0;
//	unsigned char cntNoteRepeated = 0;
	unsigned char waitTime = 0;

	double noteSequence[13] = {523.25, 587.33, 523.25, 587.33, 523.25, 587.33, 523.25, 587.33, 523.25, 587.33, 523.25,      440.00, 440.00}; //13
                        /*523.25, 587.33, 440.00, //3
                        523.25,587.33,523.25,587.33,523.25,587.33,523.25,587.33,523.25,587.33,523.25,587.33,523.25,587.33,523.25,587.33,523.25,698.46, 440.00, //19
                        523.25, 523.25, 523.25, 523.25. 523.25,         698.46, 698.46, 440.00, 440.00          //9
                        523.25, 659.25, 523.25, 523.25, 523.25, 523.25, 523.25,         440.00, 392.00, 349.23, 349.23 };   //11
        */
	int adjacentNoteTime[12]= { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 5,    25};  //12
                        /*     0, 23,           33  //3
                             0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 25,       25,     3, //19
                             3, 13, 1, 1, 3,     3, 21,         3, 19 //9
                             1, 1, 1, 1, 1, 1, 27,      1, 1, 1, 0 };   //12
                        */

        switch (STATE) {
		case start:
			STATE = checkButton;
			break;
		case checkButton:
			if (temp == 0x01) {
				STATE = playSong;
			}
			else {
				STATE = checkButton;
			}
			break;
		case playSong:
		//	if (cntNoteRepeated < noteTimeHeld[i]) {  //repeat note as long as it is held
		//		++cntNoteRepeated;
		//		STATE = playSong;
		//	}
		//	else {
		//		cntNoteRepeated = 0;
				STATE = timeBetweenNotes;
		//	}
			break;
		case timeBetweenNotes:
			if (waitTime < adjacentNoteTime[i]) {
				++waitTime;
				STATE = timeBetweenNotes;
			}
			else if (i == 13) { //might be dif #
				waitTime = 0;
				i = 0;
				STATE = checkButton;
			}
			else {	
				waitTime = 0;
				++i;
				STATE = playSong;
			}
			break;
	}
	switch (STATE) {
		case start:
			break;
		case checkButton:
			set_PWM(0.00);
			break;
		case playSong:
			set_PWM(noteSequence[i]);
			break;
		case timeBetweenNotes:
			set_PWM(0.00);
			break;
        }
}

int main(void) {
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
    TimerSet(100);
    TimerOn();

    PWM_on();
    while (1) {
        playMelody();
        while (!TimerFlag);
        TimerFlag = 0;	
    }
    return 1;
}

