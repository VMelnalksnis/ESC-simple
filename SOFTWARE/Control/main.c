/*
 *	main.c
 *
 *	Created	: 08.04.2018 14:47:38
 *	Author	: Valters Melnalksnis
 *	PCB		: V1A
 */

#include "control.h"

volatile uint16_t ctrl_length = 0;
volatile uint8_t ctrl_changed = 0;
volatile uint8_t blink_ovrflw = 0;
volatile uint8_t blink_counter = 0;

int main(void)
{
	// Disable Global Interrupts.
	cli();
	
	InitIO();
	InitTimers();
	InitInterrupts();
	
	// Set 2nd test pin HIGH, indicating startup state.
	TEST_PORT ^= (1 << TEST_PIN_2);
	// Wait for control pin to be LOW.
	while ((PIND & (1 << CTRL_PIN)) == 1)
	{
		;
	}

	// Set 2nd test pin LOW, indicating normal operation.
	TEST_PORT ^= (1 << TEST_PIN_2);
	
	// Enable Global Interrupts.
	sei();

	while (1)
	{
		if (ctrl_changed == 1)
		{
			// Clamp the control signal to the min/max values.
			if (ctrl_length < CTRL_MIN)
				ctrl_length = CTRL_MIN;

			if (ctrl_length > CTRL_MAX)
				ctrl_length = CTRL_MAX;
			
			// Blink interval from 0.3 to 0.6 seconds.
			blink_ovrflw = ctrl_length / 800;
			
			// Toggle LED
			if (blink_counter >= blink_ovrflw)
			{
				TEST_PORT ^= (1 << TEST_PIN_1);
				blink_counter = 0;
			}
		}
	}
}

void InitIO()
{
	// Set test pins as outputs.
	TEST_DDR = (1 << TEST_PIN_1) | (1 << TEST_PIN_2);
	// Set outputs to LOW.
	TEST_PORT = (0 << TEST_PIN_1) | (0 << TEST_PIN_2);
	// Set control pin as an input.
	CTRL_DDR = 0;
	// Disable pull-up resistor
	CTRL_PORT = 0;
}

void InitTimers()
{
	// Disable Compare Output Mode, Normal Mode of Operation.
	TCCR1A = 0;
	// Normal Mode of Operation, Disable Input Capture, No clock source.
	TCCR1B = 0;
	// Disable Force Output Compare.
	TCCR1C = 0;
	// Enable Overflow Interrupt.
	TIMSK1 = (1 << TOIE1);
	
	// Disable Compare Output Mode, Normal Mode of Operation.
	TCCR0A = 0;
	// Disable Force Output Compare, 1024 pre-scaler.
	TCCR0B = (1 << CS02) | (0 << CS01) | (1 << CS00);
	// Enable Overflow interrupt.
	TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
}

void InitInterrupts()
{
	// Any logical change on INT0 generates an interrupt request.
	EICRA = (1 << ISC00);
	// Enable External Interrupt Request 0.
	EIMSK = (1 << INT0);
}

ISR(INT0_vect)
{
	if ((PIND & (1 << CTRL_PIN)) == 1)
	{
		// Control pin changed from LOW to HIGH.
		// Start timer by selecting clock source with no pre-scaler.
		TCCR1B = (1 << CS10);
	}
	else
	{
		// Stop timer.
		TCCR1B = 0;
		// Read current TIMER1 value.
		ctrl_length = TCNT1;
		// Clear TIMER1 value.
		TCNT1 = 0;
		// Set the control changed flag.
		ctrl_changed = 1;
		// Set 2nd test pin LOW, indicating normal operation.
		TEST_PORT &= ~(1 << TEST_PIN_2);
	}
}

ISR(TIMER0_OVF_vect)
{
	blink_counter++;
}

ISR(TIMER1_OVF_vect)
{
	// Disable Global Interrupts.
	cli();
	// Set 2nd test pin HIGH, indicating error state.
	TEST_PORT |= (1 << TEST_PIN_2);
}