/*
 *	main.c
 *
 *	Created	: 08.04.2018 14:47:38
 *	Author	: Valters Melnalksnis
 *	PCB		: V1A, V2A
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// PB1:
//	-) PCINT1	- Pin Change Interrupt 1
//	-) OC1A		- Timer/Counter1 Output Compare Match A Output
// PB2:
//	-) PCINT2	- Pin Change Interrupt 2
//	-) OC1B		- Timer/Counter1 Output Compare Match B Output
//	-) SS		- SPI Bus Master Slave select

#define V2A

#ifdef V1A

#define TEST_PIN_1		PINB1
#define TEST_PIN_2		PINB2
#define TEST_DDR		DDRB
#define TEST_PORT		PORTB

#endif

#ifdef V2A

#define TEST_PIN_1		PINC0
#define TEST_PIN_2		PINC1
#define TEST_DDR		DDRC
#define TEST_PORT		PORTC

#endif

char enableLED = 1;
int counter = 1;

volatile uint8_t tot_overflow;


int main(void)
{
	// Set test pins as outputs.
	TEST_DDR = (1 << TEST_PIN_1) | (1 << TEST_PIN_2);
	// Set outputs to LOW.
	TEST_PORT = (1 << TEST_PIN_1) | (0 << TEST_PIN_2);

	// Initialize timer
	// Setup Timer0 in normal mode of operation and 1024 prescaler
	TCCR0A = 0;
	TCCR0B = (0 << CS02) | (1 << CS01) | (1 << CS00);
	// Enable Overflow Interrupt Enable
	TIMSK0 = (1 << TOIE0);
	// Initialize counter
	TCNT0 = 0;
	sei();

	while (1)
	{
		if (tot_overflow >= counter)
		{
			TEST_PORT ^= (1 << TEST_PIN_1) | (1 << TEST_PIN_2);
			tot_overflow = 0;
			counter++;
			if (counter >= 200)
			{
				counter = 1;
			}
		}
	}
}

ISR(TIMER0_OVF_vect)
{
	tot_overflow++;
}
