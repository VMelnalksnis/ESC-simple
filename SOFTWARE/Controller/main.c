/*
 * Controller.c
 *
 * Created: 01.05.2018 15:46:03
 * Author : Valters Melnalksnis
 */

#include "Controller.h"

/*	Time at which the current control signal began.	*/
volatile uint16_t control_start;
/*	Width of the last control signal pulse, in clock cycles.	*/
volatile uint16_t control_width;

int main(void)
{
    /*	Set test/status pins as outputs.	*/
	TEST_DDR = (1 << TEST_PIN_1) | (1 << TEST_PIN_2);
	/*	Initialize test/status pins to LOW (LEDs OFF).	*/
	TEST_PORT = 1;

	/*	Set control signal pin as input.	*/


	/*	Initialize Timer1, which is used to measure the control signal.	*/
	/*	Disconnect compare outputs, normal mode of operation,
		enable input capture noise canceler, input capture positive edge,
		pre-scaler set to 1, disable force output compare.	*/
	TCCR1A = 0;
	TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS10);
	TCCR1C = 0;
	/*	Enable interrupt capture interrupt.	*/
	TIMSK1 = (1 << ICIE1) | (0 << TOIE1);

	sei();
    while (1)
    {
		TEST_PORT ^= 1;
    }
}

/*	Timer1 Input Capture interrupt service routine - executed when the select edge is
 *	detected on the input capture pin. Used to measure the positive pulse width of the
 *	input control signal that determines the target speed.
 */
ISR(TIMER1_CAPT_vect)
{
	/*	Read the captured value first so it isn't overwritten.	*/
	uint16_t temp = ICR1;

	if (TCCR1B & (1 << ICES1))
	{
		/*	Save the pulse start time.	*/
		control_start = temp;
	}
	else
	{
		/*	Save the pulse width.	*/
		control_width = temp - control_start;
		/*	Save the pulse start time.	*/
		control_start = temp;
	}

	/*	Toggle the input capture edge, so the opposite edge is captured next.	*/
	TCCR1B ^= (1 << ICES1);
	/*	Clear the Input Capture Flag so the next edge can be captured.	*/
	TIFR1 = (1 << ICF1);
}