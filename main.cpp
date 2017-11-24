///////////////////////////////
/*CODE FOR MILESTONE PROJECT */
///////////////////////////////

#include <avr/io.h>
#include "usart_ATmega1284.h"
#include "timer.h"

/* GLOBAL VARIABLES */
unsigned char run;			//Determines whether to turn on the pump or not

enum States{wait, pump} state;

void Tick() {
	switch(state) {
		case wait:
			if (run == 0) {
				state = wait;
			} else if (run == 1) {
				state = pump;
				PORTC = 0x01;
			} else {
				state = wait;
			}
			break;
		case pump:
			if (run == 1) {
				state = pump;
			} else if (run == 0) {
				state = wait;
				PORTC = 0x00;
			} else {
				state = wait;
			}
			break;
		default:
			break;
	}
	
	switch(state) {
		case wait:
			if (USART_HasReceived(1)) {
				run = USART_Receive(1);
				USART_Flush(1);
			}
			break;
		case pump:
			if (USART_HasReceived(1)) {
				run = USART_Receive(1);
				USART_Flush(1);
			}
			break;
		default:
			break;
	}
}

int main(void)
{
	//Uses second ATmega1284
	DDRA = 0x00; PORTA = 0x00;	//Buttons for testing (Placeholders for water sensor)
	DDRC = 0xFF; PORTC = 0x00;  //LED for testing (Placeholder for DC Motor)
	DDRD = 0x0A; PORTD = 0x00;	//For USART
	
	//Initializations
	initUSART(1);
	run = 0;
	state = wait;
	
	TimerSet(200);
	TimerOn();
	
	while (1)
	{
		//unsigned char a = PINA;
		if (USART_IsSendReady(1)) {
			USART_Send(PINA, 1);
			while (!USART_HasTransmitted(1)) {}
		}
		Tick();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}

