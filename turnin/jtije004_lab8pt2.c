/*	Author: Jeana Tijerina
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
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

enum STATES {start, turnOff, turnOn, increment, decrement} STATE;
double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char temp = 0x00;
void playNotes() {
        temp = ~PINA;
	unsigned char i = 0;

        switch (STATE) {
		case start:
			STATE = turnOff;
			break;
		case turnOff:
			if (temp == 0x01) {
				STATE = turnOn;
			}
			else {
				STATE = turnOff;
			}
			break;
		case turnOn:
			if (temp == 0x01) {
				STATE = turnOff;
			}	
			else {
				if (temp == 0x02) {
					STATE = increment;
				}
				else if (temp == 0x04) {
					STATE = decrement;
				}
				else {
					STATE = turnOn;
				}
			}
                        break;
		case increment:
			if (temp == 0x01) {
				STATE = turnOff;
			}
			else {	
				if (i < 8){
					++i;
				}
				STATE = turnOn;
			}
			break;
                case decrement:
			if(temp == 0x01){
				STATE = turnOff;
                        }
			else {
				if (i > 0) {
					--i;
				}
				STATE = turnOn;
			}
			break;
        }
	switch (STATE) {
		case start:
			break;
                case turnOff:
                      	set_PWM(0.00);
                        break;
                case turnOn:
			set_PWM(notes[0]);
                        break;
                case increment:
			set_PWM(notes[i]);
                        break;
                case decrement:
			set_PWM(notes[i]);
			break;
	}
}

int main(void) {
    /* Insert DDR and PORT initializations */
        DDRA = 0x00; PORTA = 0xFF;
        DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */
        
    PWM_on();
    while (1) {
        playNotes();
    }
    return 1;
}
