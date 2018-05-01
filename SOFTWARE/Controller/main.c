/*
 * Controller.c
 *
 * Created: 01.05.2018 15:46:03
 * Author : Valters Melnalksnis
 */

#include "Controller.h"

volatile uint8_t current_state = 0;

const uint8_t driver_states_on[] = {	DRIVER_STATE_ON_1, DRIVER_STATE_ON_2,
										DRIVER_STATE_ON_3, DRIVER_STATE_ON_4,
										DRIVER_STATE_ON_5, DRIVER_STATE_ON_6 };
const uint8_t driver_states_off[] = {	DRIVER_STATE_OFF_1, DRIVER_STATE_OFF_2,
										DRIVER_STATE_OFF_3, DRIVER_STATE_OFF_4,
										DRIVER_STATE_OFF_5, DRIVER_STATE_OFF_6 };

/*	Time at which the current control signal began.	*/
volatile uint16_t control_start;
/*	Width of the last control signal pulse, in clock cycles.	*/
volatile uint16_t control_width;

volatile uint8_t driver_frequency;

int main(void)
{
	InitIO();

	/*	Initialize Timer1, which is used to measure the control signal.	*/
	/*	Disconnect compare outputs, normal mode of operation,
		enable input capture noise canceler, input capture positive edge,
		pre-scaler set to 1, disable force output compare.	*/
	TCCR1A = 0;
	TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS10);
	TCCR1C = 0;
	/*	Enable interrupt capture interrupt.	*/
	TIMSK1 = (1 << ICIE1) | (0 << TOIE1);

	/*	Enable global interrupts.	*/
	sei();

	while (1)
	{

	}
}

/*
 *	Initialize all the digital and analog inputs and outputs.
 */
void InitIO()
{
	/*	Set CONTROL and ZC_B as inputs.	*/
	DDRB = 0;
	/*	Disable pull-up resistors for CONTROL and ZC_B.	*/
	PORTB = 0;

	/*	Set test pins as outputs, ZC_C as input.	*/
	DDRC = (0 << ZC_C_PIN) | (1 << TEST_PIN_2) | (1 << TEST_PIN_1);
	/*	Set test pins to LOW, disable pull-up resistor on ZC_C.	*/
	PORTC = (0 << ZC_C_PIN) | (0 << TEST_PIN_2) | (0 << TEST_PIN_1);

	/*	Set driver control pins as outputs, ZC_A as input.	*/
	DDRD =	(0 << ZC_A_PIN) |
			(1 << PHASE_A) | (1 << SD_A) |
			(1 << PHASE_B) | (1 << SD_B) |
			(1 << PHASE_C) | (1 << SD_C);
	/*	Set driver control pins to LOW, disable pull-up resistor on ZC_A.	*/
	PORTD =	(0 << ZC_A_PIN) |
			(0 << PHASE_A) | (0 << SD_A) |
			(0 << PHASE_B) | (0 << SD_B) |
			(0 << PHASE_C) | (0 << SD_C);

	/*	AREF as voltage reference, left adjusted, ADC7 (current).	*/
	ADMUX = (1 << ADLAR) | (0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
	/*	ADC disabled, start conversion, enable interrupt, pre-scaler 128.	*/
	ADCSRA =	(1 << ADEN) | (1 << ADSC) | (0 << ADATE) | (1 << ADIE) |
				(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	/*	Disable analog comparator multiplexer, free running mode.	*/
	ADCSRB = 0;
}

void BlindStart()
{
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(200);
	current_state++;
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(150);
	current_state++;
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(120);
	current_state++;
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(100);
	current_state++;
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(90);
	current_state++;
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(80);
	current_state = 0;
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(70);
	current_state++;
	DRIVER_PORT = driver_states_on[current_state];
	_delay_ms(60);
	current_state++;
}

/*
 *	Timer1 Input Capture interrupt service routine - executed when the select edge is
 *	detected on the input capture pin. Used to measure the positive pulse width of the
 *	input control signal that determines the target speed.
 *
 *	control_width:
 *	60 cycles (3.75탎) if rising edge, 70 cycles (4.375탎) if falling edge.
 *
 *	driver_frequency:
 *	76 cycles (4.75탎) if rising edge, 121 cycles (7.5625탎) if falling edge.
 */
ISR(TIMER1_CAPT_vect)
{
	/*	Read the captured value first so it isn't overwritten.	*/
	uint16_t temp = ICR1;

	if ((TCCR1B & (1 << ICES1)) == 0)
	{
		/*	Falling edge.	*/
		/*	Save the pulse width.	*/
		//control_width = temp - control_start;
		driver_frequency = (temp - control_start - 16000) / 163 + 1;
	}

	/*	Save the pulse start time.	*/
	control_start = temp;
	/*	Toggle the input capture edge, so the opposite edge is captured next.	*/
	TCCR1B ^= (1 << ICES1);
	/*	Clear the Input Capture Flag so the next edge can be captured.	*/
	TIFR1 = (1 << ICF1);
}

/*
 *	Analog Digital Converter interrupt service routine - executed when the ADC has
 *	finished a conversion. Variable control_start gets zeroed each time TIMER1_CAPT_vect
 *	is executed if ADC interrupt is enabled and this ISR isn't present, thus it isn't
 *	possible to measure control pulse width. TODO - check if it is the same outside of
 *	simulation.
 */
ISR(ADC_vect)
{

}