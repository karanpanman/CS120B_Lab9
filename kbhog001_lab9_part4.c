  /* Author: Karan Bhogal
 * Lab Section: 21
 * Assignment: Lab #9  Exercise #4
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/1i3wueYMy4X2aVPdNEl4csM4QT35m5DKd/view?usp=sharing
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;

unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

unsigned char threeLEDs = 0x00;

unsigned int c1 = 0;

enum LA_State { S1, S2, S3 } Light;

void Triple ()
{
   switch (Light){
	
	case S1:
		if (c1 == 300){
			c1 = 0;
			Light = S2;
		}
		else {
			c1++;
			Light = S1;
		}
		break;

	case S2:
		if (c1 == 300){
			c1 = 0;
			Light = S3;
		}
		else{
			c1++;
			Light = S2;
		}
		break;

	case S3:
		if (c1 == 300){
			c1 = 0;
			Light = S1;
		}
		else{
			c1++;
			Light = S3;
		}
		break;

	default:
		Light = S1;
		break;
   }

   switch (Light){

	case S1:
		threeLEDs = (threeLEDs & 0x08) | 0x01;
		break;

	case S2:
		threeLEDs = (threeLEDs & 0x08) | 0x02;
		break;

	case S3:
		threeLEDs = (threeLEDs & 0x08) | 0x04;
		break;

	default:
	break;

   }
}

unsigned int i = 0;

enum LB_States { On, Off } Check;

void Single()
{
   switch(Check){

	case On:
		if ( i == 1000){
			i = 0;
			Check = Off;
		}
		else{
			i++;
			Check = On;
		}
		break;

	case Off:
		if ( i == 1000){
			i = 0;
			Check = On;
		}
		else{
			i++;
			Check = Off;
		}
		break;

	default:
		Check = On;
		break;
   }

   switch (Check){

	case On:
		threeLEDs = (threeLEDs & 0x07) | 0x08;
		break;

	case Off: 
		threeLEDs = (threeLEDs & 0x07);
		break;

	default:
		break;
   }
}

enum LC_States { Output } St;

void Out ()
{
   switch (St){
	case Output:
		St = Output;
		break;
	
	default:
		St = Output;
		break;
   }

   switch (St){
	case Output:
		PORTB = threeLEDs;
		break;

	default:
		break;
   }
}

unsigned int num = 0;
unsigned int poll = 2;

enum LD_States { Wait, on, off } Pos;

void Speaker ()
{
	unsigned char tmp = ~PINA;
   switch (Pos){
        case Wait:
		if ( tmp == 0x04 ){	
                	Pos = on;
		}
		else{
			Pos = Wait;
		}
                break;

	case on:
		if ( (tmp == 0x04) && (num == poll) ){
			num = 0;
			Pos = off;
		}
		else if ( (tmp == 0x04) ) {
			num++;
			Pos = on;
		}
		else {
			num = 0;
			Pos = Wait;
		}
		break;

	case off:
                if ( (tmp == 0x04) && (num == poll) ){
                        num = 0;
                        Pos = on;
                }
                else if ( (tmp == 0x04) ) {
                        num++;
                        Pos = off;
                }
                else {
                        num = 0;
                        Pos = Wait;
                }
                break;

        default:
                Pos = Output;
                break;
   }

   switch (Pos){
        case Wait:
                threeLEDs = threeLEDs & 0x0F;
                break;

	case on:
		threeLEDs = threeLEDs | 0x10;
		break;
	case off:
		threeLEDs = threeLEDs & 0x0F;
		break;

        default:
                break;
   }
}


enum LE_States { wait, inc, dec, wait2 } Spot;

void Poll ()
{
	unsigned char tmp = ~PINA;
   switch (Spot){
        case wait:
		if ( tmp == 0x01){
			Spot = inc;
		}
		else if ( tmp == 0x02){
                	Spot = dec;
		}
		else{
			Spot = wait;
		}
                break;

	case inc:
		Spot = wait2;
		break;

	case dec:
		Spot = wait2;
		break;

	case wait2:
		if ( tmp != 0x00 ){
			Spot = wait2;
		}
		else{
			Spot = wait;
		}
		break;

        default:
                St = Output;
                break;
   }

   switch (Spot){
        case wait:
                break;

	case inc:
		poll = poll + 1;
		break;

	case dec:
		poll = poll - 1;
		break;

	case wait2:
		break;

        default:
                break;
   }
}



int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    /* Insert your solution below */

	TimerSet(1);
	TimerOn();

	Light = S1;
	Check = On;
	St = Output;
	Pos = Wait;
	Spot = wait;

    while (1) {

	while(!TimerFlag);
	TimerFlag = 0;
	Triple();
	Single();
	Speaker();	
	Poll();
	Out();

    }

    return 1;
}
