/*
 *	main.c
 *
 *	Created	: 08.04.2018
 *	Author	: Valters Melnalksnis
 *	PCB		: V1A
 */

#include "control.h"

/*	This counter is used to blink an LED on TEST_PIN_1.
 *	The frequency of the blinking is dependent on the control signal.
 */
volatile uint8_t overflow_counter = 0;
/*	The above counter is compared to this value to decided whether TEST_PIN_1
 *	should be toggled. This value is proportional to the width of the last
 *	control signal pulse.
 */
volatile uint8_t blink_period = 0;


int main(void)
{
	/*	Initialization functions.	*/
	InitIO();
	InitTimers();
	InitInterrupts();

	/*	Enable global interrupts.	*/
	sei();
	
	/*	Run this forever.	*/
	while (1)
	{
		/*	Do nothing since everything is handled is ISR.	*/
	}/*	End while(1).	*/

}/*	End main(void).	*/

/*	Initialize all the used inputs/outputs.	*/
void InitIO(void)
{
	/*	Set the test pins as outputs.	*/
	TEST_DDR = (1 << TEST_PIN_1) | (1 << TEST_PIN_2);
	/*	Initialize the test outputs to LOW.	*/
	TEST_PORT = (0 << TEST_PIN_1) | (0 << TEST_PIN_2);
	/*	Set control signal pin as an input.	*/
	CTRL_DDR = 0;
	/*	Disable internal pull-up resistor for the control signal.	*/
	CTRL_PORT = 0;
}

/*	Initialize all the used timers/counters.
 *	
 *	Timer1 is used to measure the width of the control signal pulse.
 *	Since the maximum width of the pulse is 2ms, the timer can be used with
 *	a pre-scaler of 1 for maximum resolution. The output compare values A and B
 *	are set to the minimum and maximum pulse width, respectively.
 *	
 *	Timer0 is used to blink an LED on TEST_PIN_1.
 *	Since the blinking frequency change should be distinguishable by the 
 *	human eye, a pre-scaler of 1024 is used for maximum period of 4.2s when
 *	using an 8-bit overflow counter.
 */
void InitTimers(void)
{
	/*	Initialize Timer1 - disable compare output mode, set normal mode of operation,
		select no clock source.	*/
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1C = 0;
	/*	Set Output Compare A to min pulse width (1ms).	*/
	OCR1A = CTRL_MIN;
	/*	Set Output Compare B to max pulse width (2ms).	*/
	OCR1B = CTRL_MAX;

	/*	Initialize Timer0 - disable compare output mode, set normal mode of operation,
		select clock with a pre-scaler of 1024.	*/
	TCCR0A = 0;
	TCCR0B = TIMER0_PRESC;
}

/*	Initialize all the used interrupts.
 *	
 *	Timer0 overflow interrupt is used control the blinking frequency of TEST_PIN_1.
 *	
 *	Timer1 output compare interrupts are used to clamp the control signal pulse width
 *	to the predefined minimum and maximum values.
 *
 *	External interrupt 0 is used to detect the rising and falling edge of the control
 *	signals pulse.
 */
void InitInterrupts(void)
{
	/*	Enable Overflow interrupt for Timer0.	*/
	TIMSK0 = (1 << TOIE0);
	/*	Enable output compare interrupts for Timer1.	*/
	TIMSK1 = (1 << OCIE1B) | (1 << OCIE1A) | (0 << TOIE1);

	/*	Any logical change on INT0 generates an interrupt request.	*/
	EICRA = (0 << ISC01) | (1 << ISC00);
	/*	Enable External Interrupt Request 0.	*/
	EIMSK = (1 << INT0);
}

/*	This is the Timer0 overflow interrupt service routine; runs every time the Timer0
 *	overflows. Each time overflow_counter is incremented and checked against
 *	blink_perdiod; if the former is greater than the latter, TEST_PIN_1 is toggled and
 *	overflow_counter is reset.
 */
ISR(TIMER0_OVF_vect)
{
	overflow_counter++;
	
	if (overflow_counter >= blink_period)
	{
		/*	Toggle the logic state of TEST_PIN_1.	*/
		TEST_PORT ^= (1 << TEST_PIN_1);
		/*	Reset the counter.	*/
		overflow_counter = 0;
	}
}

/*	This is the external interrupt 0 interrupt service routine; runs every time the 
 *	logic level of the corresponding pin changes. If a rising edge is detected, Timer1
 *	is started, otherwise the pulse width is clamped between minimum and maximum, and
 *	blink_period is set according to the value of Timer1.
 */
ISR(INT0_vect)
{
	if ((CTRL_PINS & (1 << CTRL_PIN)) == 1)
	{
		/*	Control signals rising edge; start Timer1.	*/
		TCCR1B = TIMER1_PRESC;
	}
	else
	{
		/*	Control signals falling edge.	*/
		if ((TIFR1 & (1 << OCF1A)) == 0)
		{
			/*	Timer1 didn't exceed minimum pulse width.
				Clamp the blink period to the minimum value.	*/
			blink_period = CTRL_MIN / CTRL_SCALE;
		}
		else if ((TIFR1 & (1 << OCF1B)) == 1)
		{
			/*	Timer1 exceeded maximum pulse width.
				Clamp the blink period to the maximum value and clear the Timer1
				output compare B flag.	*/
			blink_period = CTRL_MAX / CTRL_SCALE;
			TIFR1 = (1 << OCF1B);
		}
		else
		{
			/*	Timer1 is within bounds.
				Clear the Timer1 output compare A flag.	*/
			blink_period = (TCNT1 / CTRL_SCALE);
			TIFR1 = (1 << OCF1A);
		}
		
		/*	Select no clock source for Timer1 to stop it.	*/
		TCCR1B = 0;
		/*	Clear Timer1.	*/
		TCNT1 = 0;
	}
}