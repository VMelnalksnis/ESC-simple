/*
 *	main.c
 *
 *	Created	: 08.04.2018 14:47:38
 *	Author	: Valters Melnalksnis
 *	PCB		: V1A
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

/*	Pins used to control the 3 half-bridge drivers.	*/
#define DRIVER_DDR		DDRD
#define DRIVER_PORT		PORTD
#define SD_C			PIND0
#define PHASE_C			PIND1
#define SD_B			PIND3
#define PHASE_B			PIND4
#define SD_A			PIND5
#define PHASE_A			PIND6

#define DRIVER_STATE_1	((1 << SD_A) | (1 << PHASE_A) | (1 << SD_B))
#define DRIVER_STATE_2	((1 << SD_A) | (1 << PHASE_A) | (1 << SD_C))
#define DRIVER_STATE_3	((1 << SD_B) | (1 << PHASE_B) | (1 << SD_C))
#define DRIVER_STATE_4	((1 << SD_B) | (1 << PHASE_B) | (1 << SD_A))
#define DRIVER_STATE_5	((1 << SD_C) | (1 << PHASE_C) | (1 << SD_A))
#define DRIVER_STATE_6	((1 << SD_C) | (1 << PHASE_C) | (1 << SD_B))

const uint8_t driver_states[] = {	DRIVER_STATE_1, DRIVER_STATE_2, DRIVER_STATE_3,
									DRIVER_STATE_4, DRIVER_STATE_5, DRIVER_STATE_6 };
volatile uint8_t current_state = 0;

char enableLED = 1;
int counter = 1;

volatile uint16_t tot_overflow;


int main(void)
{
	// Set test pins as outputs.
	TEST_DDR = (1 << TEST_PIN_1) | (1 << TEST_PIN_2);
	// Set outputs to LOW.
	TEST_PORT = (1 << TEST_PIN_1) | (0 << TEST_PIN_2);
	
	/*	Set driver control pins as outputs.	*/
	DRIVER_DDR = (1 << SD_C) | (1 << PHASE_C) | (1 << SD_B) | (1 << PHASE_B) | (1 << SD_A) | (1 << PHASE_A);
	/*	Close phase A high-side MOSFET and phase B low-side MOSFET.	*/
	DRIVER_PORT = 0;
	
	// Initialize timer
	// Setup Timer0 in normal mode of operation and 1024 prescaler
	TCCR0A = 0;
	TCCR0B = (0 << CS02) | (0 << CS01) | (1 << CS00);
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
			if (counter >= (UINT16_MAX - 1))
			{
				counter = 1;
			}
		}
	}
}

ISR(TIMER0_OVF_vect)
{
	tot_overflow++;
	
	if (current_state == 5)
	{
		current_state = 0;
	}
	else
	{
		current_state++;
	}
	
	DRIVER_PORT = driver_states[current_state];
}
