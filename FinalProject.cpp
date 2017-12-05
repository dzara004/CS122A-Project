/////////////////////////////////
/*CODE FOR 80-90 POINT PROJECT */
/////////////////////////////////

#include <avr/io.h>
#include "lcd.h"
#include "usart_ATmega1284.h"
#include "timer.h"
#include "bit.h"

/* Library for OLEDs found here: https://github.com/tibounise/SSD1306-AVR */
#include "SSD1306-AVR-master/I2C.h"
#include "SSD1306-AVR-master/I2C.cpp"
#include "SSD1306-AVR-master/SSD1306.cpp"
#include "SSD1306-AVR-master/Framebuffer.cpp"
#include "SSD1306-AVR-master/simulator/I2C.h"

/* GLOBAL VARIABLES */
unsigned char waterValue;		//Holds the current level of the water
unsigned char pump;			//Determines whether to turn on the pump or not
Framebuffer fb;				//Variable for writing to OLED

/* OLED FUNCTIONS */
void draw0(Framebuffer);
void draw25(Framebuffer);
void draw50(Framebuffer);
void draw75(Framebuffer);

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
			draw0(fb);
			break;
		case low:
			PORTC = 0x10;
			draw25(fb);
			break;
		case middle:
			PORTC = 0x30;
			draw50(fb);
			break;
		case high:
			pump = 1;
			PORTC = 0x70;
			draw75(fb);
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
	DDRC = 0xFF; PORTC = 0x00;  	//For LED and Blue OLED
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

void draw0(Framebuffer fb) {
	uint8_t zero[16] = {
		0b01111110,
		0b11111111,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		//Next 8 rows
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11111111,
		0b01111110
	};
	uint8_t percent[16] = {
		0b00000111,
		0b11000101,
		0b11000111,
		0b01100000,
		0b01100000,
		0b00110000,
		0b00110000,
		0b00011000,
		//Next 8 rows
		0b00011000,
		0b00001100,
		0b00001100,
		0b00000110,
		0b00000110,
		0b11100011,
		0b10100011,
		0b11100000
	};
	for (int i = 0; i < sizeof(zero); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((zero[i] >> p) & 0x1) {
				fb.drawPixel(p+25,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(percent); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((percent[i] >> p) & 0x1) {
				fb.drawPixel(p+35,i+25);
			}
		}
	}
	fb.drawHLine(75, 60, 48);
	fb.drawVLine(70, 0, 63);
	fb.drawHLine(70, 63, 60);
	fb.drawVLine(127, 0, 63);
	for (unsigned char i = 60; i > 60; --i) {
		fb.drawHLine(75, i, 48);
	}
	fb.show();
}

void draw25(Framebuffer fb) {
	uint8_t two[16] = {
		0b00111100,
		0b01111110,
		0b11100111,
		0b11100011,
		0b11100011,
		0b11100000,
		0b11110000,
		0b01110000,
		//Next 8 rows
		0b01110000,
		0b01111000,
		0b00111100,
		0b00011110,
		0b00001111,
		0b11111111,
		0b11111111,
		0b11111111
	};
	uint8_t five[16] = {
		0b11111111,
		0b11111111,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		//Next 8 rows
		0b00111110,
		0b01110000,
		0b11100000,
		0b11100000,
		0b11100000,
		0b01110000,
		0b00111110,
		0b00000111
	};
	uint8_t percent[16] = {
		0b00000111,
		0b11000101,
		0b11000111,
		0b01100000,
		0b01100000,
		0b00110000,
		0b00110000,
		0b00011000,
		//Next 8 rows
		0b00011000,
		0b00001100,
		0b00001100,
		0b00000110,
		0b00000110,
		0b11100011,
		0b10100011,
		0b11100000
	};
	for (int i = 0; i < sizeof(two); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((two[i] >> p) & 0x1) {
				fb.drawPixel(p+25,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(five); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((five[i] >> p) & 0x1) {
				fb.drawPixel(p+35,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(percent); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((percent[i] >> p) & 0x1) {
				fb.drawPixel(p+45,i+25);
			}
		}
	}
	fb.drawHLine(75, 60, 48);
	fb.drawVLine(70, 0, 63);
	fb.drawHLine(70, 63, 60);
	fb.drawVLine(127, 0, 63);
	for (unsigned char i = 60; i > 45; --i) {
		fb.drawHLine(75, i, 48);
	}
	fb.show();
}

void draw50(Framebuffer fb) {
	uint8_t five[16] = {
		0b11111111,
		0b11111111,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		//Next 8 rows
		0b00111110,
		0b01110000,
		0b11100000,
		0b11100000,
		0b11100000,
		0b01110000,
		0b00111110,
		0b00000111
	};
	uint8_t zero[16] = {
		0b01111110,
		0b11111111,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		//Next 8 rows
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11111111,
		0b01111110
	};
	uint8_t percent[16] = {
		0b00000111,
		0b11000101,
		0b11000111,
		0b01100000,
		0b01100000,
		0b00110000,
		0b00110000,
		0b00011000,
		//Next 8 rows
		0b00011000,
		0b00001100,
		0b00001100,
		0b00000110,
		0b00000110,
		0b11100011,
		0b10100011,
		0b11100000
	};
	for (int i = 0; i < sizeof(five); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((five[i] >> p) & 0x1) {
				fb.drawPixel(p+25,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(zero); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((zero[i] >> p) & 0x1) {
				fb.drawPixel(p+35,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(percent); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((percent[i] >> p) & 0x1) {
				fb.drawPixel(p+45,i+25);
			}
		}
	}
	fb.drawHLine(75, 60, 48);
	fb.drawVLine(70, 0, 63);
	fb.drawHLine(70, 63, 60);
	fb.drawVLine(127, 0, 63);
	for (unsigned char i = 60; i > 30; --i) {
		fb.drawHLine(75, i, 48);
	}
	fb.show();
}

void draw75(Framebuffer fb) {
	uint8_t seven[16] = {
		0b11111111,
		0b11111111,
		0b11111111,
		0b11100000,
		0b11100000,
		0b01110000,
		0b01110000,
		0b00111000,
		//Next 8 rows
		0b00111000,
		0b00011100,
		0b00011100,
		0b00001110,
		0b00001110,
		0b00000111,
		0b00000111,
		0b00000111
	};
	uint8_t five[16] = {
		0b11111111,
		0b11111111,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		//Next 8 rows
		0b00111110,
		0b01110000,
		0b11100000,
		0b11100000,
		0b11100000,
		0b01110000,
		0b00111110,
		0b00000111
	};
	uint8_t percent[16] = {
		0b00000111,
		0b11000101,
		0b11000111,
		0b01100000,
		0b01100000,
		0b00110000,
		0b00110000,
		0b00011000,
		//Next 8 rows
		0b00011000,
		0b00001100,
		0b00001100,
		0b00000110,
		0b00000110,
		0b11100011,
		0b10100011,
		0b11100000
	};
	for (int i = 0; i < sizeof(seven); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((seven[i] >> p) & 0x1) {
				fb.drawPixel(p+25,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(five); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((five[i] >> p) & 0x1) {
				fb.drawPixel(p+35,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(percent); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((percent[i] >> p) & 0x1) {
				fb.drawPixel(p+45,i+25);
			}
		}
	}
	fb.drawHLine(75, 60, 48);
	fb.drawVLine(70, 0, 63);
	fb.drawHLine(70, 63, 60);
	fb.drawVLine(127, 0, 63);
	for (unsigned char i = 60; i > 15; --i) {
		fb.drawHLine(75, i, 48);
	}
	fb.show();
}
