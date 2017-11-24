/////////////////////////////////
/*CODE FOR 80-90 POINT PROJECT */
/////////////////////////////////

#include <avr/io.h>
#include "lcd.h"
#include "usart_ATmega1284.h"
#include "timer.h"
#include "bit.h"
#include "SSD1306-AVR-master/I2C.h"
#include "SSD1306-AVR-master/I2C.cpp"
#include "SSD1306-AVR-master/SSD1306.cpp"
#include "SSD1306-AVR-master/Framebuffer.cpp"
#include "SSD1306-AVR-master/simulator/I2C.h"

/* GLOBAL VARIABLES */
unsigned char waterValue;	//Holds the current level of the water
unsigned char pump;			//Determines whether to turn on the pump or not

enum States{init, none, low, middle, high} state;

void Tick() {
	switch(state) {
		case init:
			state = none;
			LCD_ClearScreen();
			LCD_DisplayString(1, "Low level");
			break;
		case none:
			if (waterValue == 0x00) {
				state = none;
			} else if (waterValue == 0x01) {
				state = low;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Exceeded low");
				LCD_DisplayString(17, "level");
			} else {
				state = init;
			}
			break;
		case low:
			if (waterValue == 0x01) {
				state = low;
			} else if (waterValue == 0x03) {
				state = middle;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Nearing full");
				LCD_DisplayString(17, "level");
			} else if (waterValue == 0x00) {
				state = none;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Low level");
			} else {
				state = init;
			}
			break;
		case middle:
			if (waterValue == 0x03) {
				state = middle;
			} else if (GetBit(waterValue, 2)) {
				state = high;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Full!");
			} else if (waterValue == 0x01) {
				state = low;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Dewatering.");
				LCD_DisplayString(17, "Almost low");
			} else {
				state = init;
			}
			break;
		case high:
			if (GetBit(waterValue, 2)) {
				state = high;
			} else if (!GetBit(waterValue, 2)) {
				state = middle;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Dewatering.");
				LCD_DisplayString(17, "Near midpoint");
			} else {
				state = init;
			}
			break;
		default:
			state = init;
			break;
	}
	
	switch(state) {
		case init:
			waterValue = 0;
			pump = 0;
			break;
		case none:
			pump = 0;
			break;
		case low:
			PORTC = 0x10;
			break;
		case middle:
			PORTC = 0x30;
			break;
		case high:
			pump = 1;
			PORTC = 0x70;
			break;
		default:
			break;
	}
}

int main(void)
{
	//Uses one ATmega1284
	DDRA = 0xFF; PORTA = 0x00;	//LCD control lines
	DDRB = 0xFF; PORTB = 0x00;	//LCD data lines
	DDRC = 0xFF; PORTC = 0x00;  //For LED and Blue OLED
	DDRD = 0x0A; PORTD = 0x00;	//For USART
	
	//Initializations
	initUSART(1);
	LCD_init();
	waterValue = 0;
	pump = 0;
	state = init;
	
	TimerSet(250);
	TimerOn();
	
	while (1)
	{
		if (USART_IsSendReady(1)) {
			USART_Send(pump, 1);
			while (!USART_HasTransmitted(1)) {}
		}
		if (USART_HasReceived(1)) {
			waterValue = USART_Receive(1);
			USART_Flush(1);
		}
		Tick();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}

