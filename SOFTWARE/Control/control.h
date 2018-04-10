/*
 *	control.h
 *
 *	Created	: 2018-04-09 14:12:05
 *	Author	: Valters Melnalksnis
 *	PCB		: V1A
 */

#include <avr/io.h>
#include <avr/interrupt.h>

// PB1:
//	-) PCINT1	- Pin Change Interrupt 1
//	-) OC1A		- Timer/Counter1 Output Compare Match A Output
// PB2:
//	-) PCINT2	- Pin Change Interrupt 2
//	-) OC1B		- Timer/Counter1 Output Compare Match B Output
//	-) SS		- SPI Bus Master Slave select
// PD2:
//	-) INT0		- External Interrupt 0 Input
//	-) PCINT18	- Pin Change Interrupt 18


#define TEST_PIN_1		PINB1
#define TEST_PIN_2		PINB2
#define TEST_DDR		DDRB
#define TEST_PORT		PORTB

#define CTRL_PINS		PIND
#define CTRL_PIN		PIND2
#define CTRL_DDR		DDRD
#define CTRL_PORT		PORTD

/*	Timer1 output compare A value corresponding to the minimum pulse width.	*/
#define CTRL_MIN		16000
/*	Timer1 output compare A value corresponding to the maximum pulse width.	*/
#define CTRL_MAX		32000

#define CTRL_SCALE		800

/*	Timer0 control register value for setting a pre-scaler of 1024.	*/
#define TIMER0_PRESC	((1 << CS02) | (0 << CS01) | (1 << CS00))
/*	Timer1 control register value for setting a pre-scaler of 1.	*/
#define TIMER1_PRESC	((0 << CS12) | (0 << CS11) | (1 << CS10))

void InitIO(void);
void InitTimers(void);
void InitInterrupts(void);