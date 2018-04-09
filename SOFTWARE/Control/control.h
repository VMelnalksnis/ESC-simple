/*
 * control.h
 *
 * Created: 2018-04-09 14:12:05
 *  Author: Valters Melnalksnis
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

#define CTRL_PIN		PIND2
#define CTRL_DDR		DDRD
#define CTRL_PORT		PORTD

#define CTRL_MIN		16000
#define CTRL_MAX		32000

void InitIO(void);
void InitTimers(void);
void InitInterrupts(void);