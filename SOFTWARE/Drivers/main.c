/*
 *	main.c
 *
 *	Created	: 2018-04-11
 *	Author	: Valters Melnalksnis
 *	PCB		: V1A
 */

#include "Drivers.h"

const uint8_t driver_states[] = {	DRIVER_STATE_1, DRIVER_STATE_2, DRIVER_STATE_3, 
									DRIVER_STATE_4, DRIVER_STATE_5, DRIVER_STATE_6 };
volatile uint8_t current_state = 0;

int main(void)
{
	InitIO();
	InitTimers();
	InitInterrupts();
	
	while (1)
	{
	}
}

void InitIO()
{
	/*	Set the half-bridge control pins as outputs.	*/
	DRIVER_DDR |=	(1 << PHASE_A) | (1 << SD_A) |
	(1 << PHASE_B) | (1 << SD_B) |
	(1 << PHASE_C) | (1 << SD_C);
	/*	Initialize the output to LOW, so that all MOSFETs are open.	*/
	DRIVER_PORT = 0;
}

void InitTimers()
{
	TCCR0A = 0;
	TCCR0B = (1 << CS02) | (0 << CS01) | (1 << CS00);
}

void InitInterrupts()
{
	/*	Enable all Pin Change Interrupts, since each zero crossing pin is
	connected to a different PCI register.	*/
	PCICR = (1 << PCIE2) | (1 << PCIE1) | (1 << PCIE0);
	/*	Enable Pin Change Interrupt on the ZC_A pin.	*/
	PCMSK2 = (1 << ZC_A_INT);
	/*	Enable Pin Change Interrupt on the ZC_C pin.	*/
	PCMSK1 = (1 << ZC_C_INT);
	/*	Enable Pin Change Interrupt on the ZC_B pin.	*/
	PCMSK0 = (1 << ZC_B_INT);
	
	/*	Enable Timer0 overflow interrupt.	*/
	TIMSK0 = (1 << TOIE0);
}

