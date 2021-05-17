/*	Author: Jeana Tijerina
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 * Demo Link: https://drive.google.com/file/d/15U7ScLxMsr0xV_ckpfd0XdmynuEUgunz/view?usp=sharing
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

enum STATES {buttonPressed, start, noteC, noteD, noteE} STATE;
unsigned char temp = 0x00;
void pushButton() {
        temp = ~PINA;
        switch (STATE) {
		case start:
			STATE = buttonPressed;
			break;
		case buttonPressed:
                        if (temp == 0x01) {                //CDE
                      		STATE = noteC;
                        }
                        else if (temp == 0x02) {
                        	STATE = noteD;
                        }
                        else if (temp == 0x04) {
				STATE = noteE;
			}
			else {
				STATE = buttonPressed;
			} 
			break;
          	case noteC:
			if (temp == 0x01) {
				STATE = noteC;
			}
			else {
				STATE = buttonPressed;
			}
	      		break;		
		case noteD:        
			if (temp == 0x02) {
                                STATE = noteD;
                        }
                        else {  
                                STATE = buttonPressed;
                        }
			break;
		case noteE:
                        if (temp == 0x04) {
                                STATE = noteE;
                        }
                        else {  
                                STATE = buttonPressed;
                        }
			break;
	}
	switch (STATE) {
		case start:
			break;
		case buttonPressed:
			set_PWM(0.00);
			break;
		case noteC:
			set_PWM(261.63);
			break;
		case noteD:
			set_PWM(293.66);
			break;
		case noteE:
			set_PWM(329.63);
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
        pushButton();
    }
    return 1;
}
