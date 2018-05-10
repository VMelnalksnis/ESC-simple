/*
 * Controller.c
 *
 * Created: 01.05.2018 15:46:03
 * Author : Valters Melnalksnis
 */

#include "Controller.h"

volatile uint8_t current_state = 0;
volatile uint8_t ovflw_counter = 0;

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
	
	/*	Initialize Timer0, which is used to control the PWM duty cycle.	*/
	/*	Disconnect compare outputs, normal mode of operation, pre-scaler of 1.	*/
	TCCR0A = 0;
	TCCR0B = (1 << CS01);
	/*	Enable output compare B, output compare B and overflow interrupt.	*/
	TIMSK0 = (1 << OCIE0B) | (1 << OCIE0A) | (0 << TOIE0);
	/*	Timer0 output compare A value; corresponds to PWM period (20kHz).	*/
	OCR0A = 50;
	/*	Timer0 output compare B value; corresponds to PWM ON width.	*/
	OCR0B = 100;
	
	/*	Initialize Timer1, which is used to measure the control signal.	*/
	/*	Disconnect compare outputs, normal mode of operation,
		enable input capture noise canceler, input capture positive edge,
		pre-scaler set to 1, disable force output compare.	*/
	TCCR1A = 0;
	TCCR1B = (1 << ICNC1) | (1 << ICES1) | (1 << CS10);
	TCCR1C = 0;
	/*	Enable interrupt capture interrupt.	*/
	TIMSK1 = (1 << ICIE1) | (0 << TOIE1);
	
	/*	Initialize Timer2, which is used to control commutation frequency.	*/
	/*	Pre-scaler 1024.	*/
	TCCR2A = 0;
	TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
	/*	Enable overflow interrupt.	*/
	TIMSK2 = (1 << TOIE2);

	/*	Enable global interrupts.	*/
	sei();

	while (1)
	{
		if (control_width < 16000)
		{
			TEST_PORT = 0;
		} 
		else if (control_width < 32000)
		{
			TEST_PORT = 1;
		}
		else
		{
			TEST_PORT = (1 << TEST_PIN_2) | (1 << TEST_PIN_1);
		}
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
	/*	ADC disabled, start conversion, disable interrupt, pre-scaler 128.	*/
	ADCSRA =	(0 << ADEN) | (1 << ADSC) | (0 << ADATE) | (0 << ADIE) |
				(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	/*	Disable analog comparator multiplexer, free running mode.	*/
	ADCSRB = 0;
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
		control_width = temp - control_start;
		if (control_width < CTRL_WIDTH_MIN)
		{
			control_width = CTRL_WIDTH_MIN;
		}
		else if(control_width > CTRL_WIDTH_MAX)
		{
			control_width = CTRL_WIDTH_MAX;
		}
		driver_frequency = (control_width - 16000) / 178 + 5;
		OCR0A = driver_frequency;
	}

	/*	Save the pulse start time.	*/
	control_start = temp;
	/*	Toggle the input capture edge, so the opposite edge is captured next.	*/
	TCCR1B ^= (1 << ICES1);
	/*	Clear the Input Capture Flag so the next edge can be captured.	*/
	TIFR1 = (1 << ICF1);
}

ISR(TIMER0_COMPB_vect)
{
	/*	New PWM cycle, set driver controls to equal to current state ON.	*/
	DRIVER_PORT = driver_states_on[current_state];
	/*	Reset the timer.	*/
	TCNT0 = 0;
}

ISR(TIMER0_COMPA_vect)
{
	/*	Set driver controls equal to current state OFF.	*/
	DRIVER_PORT = driver_states_off[current_state];
}

ISR(TIMER2_OVF_vect)
{
	ovflw_counter++;
	
	if (ovflw_counter >= 10)
	{
		
	} 
	else
	{
	}
	
	if (current_state == 6)
	{
		current_state = 0;
	} 
	else
	{
		current_state++;
	}
}

/*
 *	Analog Digital Converter interrupt service routine - executed when the ADC has
 *	finished a conversion. Variable control_start gets zeroed each time TIMER1_CAPT_vect
 *	is executed if ADC interrupt is enabled and this ISR isn't present, thus it isn't
 *	possible to measure control pulse width. TODO - check if it is the same outside of
 *	simulation.
 */
/*ISR(ADC_vect)
{

}*/