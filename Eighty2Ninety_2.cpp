/////////////////////////////////
/*CODE FOR 80-90 POINT PROJECT */
/////////////////////////////////

#include <avr/io.h>
#include "usart_ATmega1284.h"
#include "bit.h"
#include "timer.h"
#include "SSD1306-AVR-master/I2C.h"
#include "SSD1306-AVR-master/I2C.cpp"
#include "SSD1306-AVR-master/SSD1306.cpp"
#include "SSD1306-AVR-master/Framebuffer.cpp"
#include "SSD1306-AVR-master/simulator/I2C.h"

/* GLOBAL VARIABLES */
unsigned char run;			//Determines whether to turn on the pump or not
unsigned char level;		//Holds the level of the water
unsigned char phone;		//Returns the input of the phone
Framebuffer fb;				//Variable for writing to OLED
unsigned char i;			//Index for looping

/* GLOBAL ARRAYS */
unsigned char words0[8] = {'L', 'e', 'v', 'e', 'l', ' ', '0', '\n'};
unsigned char words1[8] = {'L', 'e', 'v', 'e', 'l', ' ', '1', '\n'};
unsigned char words2[8] = {'L', 'e', 'v', 'e', 'l', ' ', '2', '\n'};
unsigned char words3[8] = {'L', 'e', 'v', 'e', 'l', ' ', '3', '\n'};
	
unsigned char liters0[18] = {'B', 'e', 'l', 'o', 'w', ' ', '0', '.', '1', '8', ' ', 'L', 'i', 't', 'e', 'r', 's', '\n'};
unsigned char liters1[36] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '0', '.', '1', '8', ' ', 'L', 'i', 't', 'e', 'r', 's', ' ', 'a', 'n', 'd', ' ', '0', '.', '9', '0', ' ', 'L', 'i', 't', 'e', 'r', 's', '\n'};
unsigned char liters2[36] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '0', '.', '9', '0', ' ', 'L', 'i', 't', 'e', 'r', 's', ' ', 'a', 'n', 'd', ' ', '1', '.', '8', '5', ' ', 'L', 'i', 't', 'e', 'r', 's', '\n'};
unsigned char liters3[18] = {'A', 'b', 'o', 'v', 'e', ' ', '1', '.', '8', '5', ' ', 'L', 'i', 't', 'e', 'r', 's', '\n'};

unsigned char quarts0[18] = {'B', 'e', 'l', 'o', 'w', ' ', '0', '.', '1', '9', ' ', 'Q', 'u', 'a', 'r', 't', 's', '\n'};
unsigned char quarts1[36] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '0', '.', '1', '8', ' ', 'Q', 'u', 'a', 'r', 't', 's', ' ', 'a', 'n', 'd', ' ', '0', '.', '9', '5', ' ', 'Q', 'u', 'a', 'r', 't', 's', '\n'};
unsigned char quarts2[36] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '0', '.', '9', '5', ' ', 'Q', 'u', 'a', 'r', 't', 's', ' ', 'a', 'n', 'd', ' ', '1', '.', '9', '5', ' ', 'Q', 'u', 'a', 'r', 't', 's', '\n'};
unsigned char quarts3[18] = {'A', 'b', 'o', 'v', 'e', ' ', '1', '.', '9', '5', ' ', 'Q', 'u', 'a', 'r', 't', 's', '\n'};
	
unsigned char gallons0[19] = {'B', 'e', 'l', 'o', 'w', ' ', '0', '.', '0', '5', ' ', 'G', 'a', 'l', 'l', 'o', 'n', 's', '\n'};
unsigned char gallons1[38] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '0', '.', '0', '5', ' ', 'G', 'a', 'l', 'l', 'o', 'n', 's', ' ', 'a', 'n', 'd', ' ', '0', '.', '2', '4', ' ', 'G', 'a', 'l', 'l', 'o', 'n', 's', '\n'};
unsigned char gallons2[38] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '0', '.', '2', '4', ' ', 'G', 'a', 'l', 'l', 'o', 'n', 's', ' ', 'a', 'n', 'd', ' ', '0', '.', '4', '9', ' ', 'G', 'a', 'l', 'l', 'o', 'n', 's', '\n'};
unsigned char gallons3[19] = {'A', 'b', 'o', 'v', 'e', ' ', '0', '.', '4', '9', ' ', 'G', 'a', 'l', 'l', 'o', 'n', 's', '\n'};
	
unsigned char cups0[16] = {'B', 'e', 'l', 'o', 'w', ' ', '0', '.', '7', '6', ' ', 'C', 'u', 'p', 's', '\n'};
unsigned char cups1[32] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '0', '.', '7', '6', ' ', 'C', 'u', 'p', 's', ' ', 'a', 'n', 'd', ' ', '3', '.', '8', '0', ' ', 'C', 'u', 'p', 's', '\n'};
unsigned char cups2[32] = {'B', 'e', 't', 'w', 'e', 'e', 'n', ' ', '3', '.', '8', '0', ' ', 'C', 'u', 'p', 's', ' ', 'a', 'n', 'd', ' ', '7', '.', '8', '2', ' ', 'C', 'u', 'p', 's', '\n'};
unsigned char cups3[16] = {'A', 'b', 'o', 'v', 'e', ' ', '7', '.', '8', '2', ' ', 'C', 'u', 'p', 's', '\n'};

// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0 | 1 << WGM00);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

void PumpOff(Framebuffer);
void PumpOn(Framebuffer);

enum BTStates{standby, zero, quarter, half, full} BTState;
	
void BTTick() {
	switch(BTState) {
		case standby:
			if (!phone) {
				BTState = standby;
			} else if (phone && GetBit(level, 2)) {
				BTState = full;
			} else if (phone && GetBit(level, 1)) {
				BTState = half;
			} else if (phone && GetBit(level, 0)) {
				BTState = quarter;
			} else if (phone && !GetBit(level, 0)) {
				BTState = zero;
			} else {
				BTState = standby;
			}
			break;
		case zero:
			BTState = standby;
			phone = 0;
			break;
		case quarter:
			BTState = standby;
			phone = 0;
			break;
		case half:
			BTState = standby;
			phone = 0;
			break;
		case full:
			BTState = standby;
			phone = 0;
			break;
	}
	
	switch(BTState) {
		case standby:
			i = 0;
			break;
		case zero:
			if (phone == 0x01) {
				while (i < 8) {
					if (USART_IsSendReady(0)) {
						USART_Send(words0[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x02) {
				while (i < 18) {
					if (USART_IsSendReady(0)) {
						USART_Send(liters0[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x03) {
				while (i < 18) {
					if (USART_IsSendReady(0)) {
						USART_Send(quarts0[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x04) {
				while (i < 19) {
					if (USART_IsSendReady(0)) {
						USART_Send(gallons0[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x05) {
				while (i < 16) {
					if (USART_IsSendReady(0)) {
						USART_Send(cups0[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			}
			break;
		case quarter:
			if (phone == 0x01) {
				while (i < 8) {
					if (USART_IsSendReady(0)) {
						USART_Send(words1[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x02) {
				while (i < 36) {
					if (USART_IsSendReady(0)) {
						USART_Send(liters1[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x03) {
				while (i < 36) {
					if (USART_IsSendReady(0)) {
						USART_Send(quarts1[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x04) {
				while (i < 38) {
					if (USART_IsSendReady(0)) {
						USART_Send(gallons1[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x05) {
				while (i < 32) {
					if (USART_IsSendReady(0)) {
						USART_Send(cups1[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			}
			break;
		case half:
			if (phone == 0x01) {
				while (i < 8) {
					if (USART_IsSendReady(0)) {
						USART_Send(words2[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x02) {
				while (i < 36) {
					if (USART_IsSendReady(0)) {
						USART_Send(liters2[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x03) {
				while (i < 38) {
					if (USART_IsSendReady(0)) {
						USART_Send(quarts2[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x04) {
				while (i < 38) {
					if (USART_IsSendReady(0)) {
						USART_Send(gallons2[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x05) {
				while (i < 32) {
					if (USART_IsSendReady(0)) {
						USART_Send(cups2[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			}
			break;
		case full:
			if (phone == 0x01) {
				while (i < 8) {
					if (USART_IsSendReady(0)) {
						USART_Send(words3[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x02) {
				while (i < 18) {
					if (USART_IsSendReady(0)) {
						USART_Send(liters3[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x03) {
				while (i < 18) {
					if (USART_IsSendReady(0)) {
						USART_Send(quarts3[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x04) {
				while (i < 19) {
					if (USART_IsSendReady(0)) {
						USART_Send(gallons3[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			} else if (phone == 0x05) {
				while (i < 16) {
					if (USART_IsSendReady(0)) {
						USART_Send(cups3[i++], 0);
						while (!USART_HasTransmitted(0)) {}
					}
				}
			}
			break;
	}
}

enum pumpStates{wait, pump} pumpState;

void pumpTick() {
	switch(pumpState) {
		case wait:
			if (run == 0) {
				pumpState = wait;
			} else if (run == 1) {
				pumpState = pump;
				PORTC = 0x40;
			} else {
				pumpState = wait;
			}
			break;
		case pump:
			if (run == 1) {
				pumpState = pump;
			} else if (run == 0) {
				pumpState = wait;
				PORTC = 0x00;
			} else {
				pumpState = wait;
			}
			break;
		default:
			break;
	}
	
	switch(pumpState) {
		case wait:
			if (USART_HasReceived(1)) {
				run = USART_Receive(1);
				USART_Flush(1);
			}
			PumpOff(fb);
			break;
		case pump:
			if (USART_HasReceived(1)) {
				run = USART_Receive(1);
				USART_Flush(1);
			}
			PumpOn(fb);
			break;
		default:
			break;
	}
}

int main(void)
{
	//Uses second ATmega1284
	DDRA = 0x00; PORTA = 0x00;	//Buttons for testing (Placeholders for water sensor)
	DDRB = 0xFF; PORTB = 0x00;	//For the speaker
	DDRC = 0xFF; PORTC = 0x00;  //For DC Motor and white OLED
	DDRD = 0x0A; PORTD = 0x00;	//For USART
	
	//Initializations
	initUSART(0);
	initUSART(1);
	PWM_on();
	run = 0;
	level = 0;
	phone = 0;
	i = 0;
	pumpState = wait;
	BTState = standby;
	
	TimerSet(200);
	TimerOn();
	
	while (1)
	{
		//Sends the water level
		level = PINA;
		if (USART_IsSendReady(1)) {
			USART_Send(PINA, 1);
			while (!USART_HasTransmitted(1)) {}
		}
		
		//For the speaker
		if (level == 0x07) {
			set_PWM(900);
		} else {
			set_PWM(0);
		}
		
		//Receives input from the phone
		if (USART_HasReceived(0)) {
			phone = USART_Receive(0);
			USART_Flush(0);
		}
		pumpTick();
		BTTick();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}

void PumpOff(Framebuffer fb) {
	uint8_t P1[16] = {
		0b11111111,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11111111,
		//Next 8 rows
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011
	};
	uint8_t U[16] = {
		0b11000011,
		0b11000011,
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
		0b11100111,
		0b00111100
	};
	uint8_t M[16] = {
		0b11000011,
		0b11000011,
		0b11100111,
		0b11100111,
		0b10111101,
		0b11011011,
		0b11000011,
		0b11000011,
		//Next 8 rows
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011
	};
	uint8_t P2[16] = {
		0b11111111,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11111111,
		//Next 8 rows
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011
	};
	
	uint8_t O[16] = {
		0b01111110,
		0b11000011,
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
		0b11000011,
		0b01111110
	};
	uint8_t F1[16] = {
		0b11111111,
		0b11111111,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b11111111,
		//Next 8 rows
		0b11111111,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011
	};
	uint8_t F2[16] = {
		0b11111111,
		0b11111111,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b11111111,
		//Next 8 rows
		0b11111111,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011
	};
	for (int i = 0; i < sizeof(P1); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((P1[i] >> p) & 0x1) {
				fb.drawPixel(p+25,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(U); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((U[i] >> p) & 0x1) {
				fb.drawPixel(p+35,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(M); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((M[i] >> p) & 0x1) {
				fb.drawPixel(p+45,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(P2); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((P2[i] >> p) & 0x1) {
				fb.drawPixel(p+55,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(O); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((O[i] >> p) & 0x1) {
				fb.drawPixel(p+75,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(F1); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((F1[i] >> p) & 0x1) {
				fb.drawPixel(p+85,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(F2); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((F2[i] >> p) & 0x1) {
				fb.drawPixel(p+95,i+25);
			}
		}
	}
	fb.invert(0);
	fb.show();
}

void PumpOn(Framebuffer fb) {
	uint8_t P1[16] = {
		0b11111111,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11111111,
		//Next 8 rows
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011
	};
	uint8_t U[16] = {
		0b11000011,
		0b11000011,
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
		0b11100111,
		0b00111100
	};
	uint8_t M[16] = {
		0b11000011,
		0b11000011,
		0b11100111,
		0b11100111,
		0b10111101,
		0b11011011,
		0b11000011,
		0b11000011,
		//Next 8 rows
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011
	};
	uint8_t P2[16] = {
		0b11111111,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11111111,
		//Next 8 rows
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011,
		0b00000011
	};
	
	uint8_t O[16] = {
		0b01111110,
		0b11000011,
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
		0b11000011,
		0b01111110
	};
	uint8_t N[16] = {
		0b11000011,
		0b11000011,
		0b11000111,
		0b11000111,
		0b11001111,
		0b11001111,
		0b11011011,
		0b11011011,
		//Next 8 rows
		0b11110011,
		0b11110011,
		0b11100011,
		0b11100011,
		0b11000011,
		0b11000011,
		0b11000011,
		0b11000011
	};
	for (int i = 0; i < sizeof(P1); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((P1[i] >> p) & 0x1) {
				fb.drawPixel(p+25,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(U); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((U[i] >> p) & 0x1) {
				fb.drawPixel(p+35,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(M); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((M[i] >> p) & 0x1) {
				fb.drawPixel(p+45,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(P2); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((P2[i] >> p) & 0x1) {
				fb.drawPixel(p+55,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(O); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((O[i] >> p) & 0x1) {
				fb.drawPixel(p+80,i+25);
			}
		}
	}
	for (int i = 0; i < sizeof(N); ++i) {
		for (int p = 0; p < 10; ++p) {
			if ((N[i] >> p) & 0x1) {
				fb.drawPixel(p+90,i+25);
			}
		}
	}
	fb.invert(1);
	fb.show();
}
