/*
 *	Drivers.h
 *
 *	Created	: 2018-04-11
 *	Author	: Valters Melnalksnis
 *	PCB		: V1A
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

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

/*	Pin used to detect zero crossing for phase A. PCINT23.	*/
#define ZC_A_DDR		DDRD
#define ZC_A_PORT		PORTD
#define ZC_A_PINS		PIND
#define ZC_A_PIN		PIND7
#define ZC_A_INT		PCINT23

/*	Pin used to detect zero crossing for phase B. PCINT0.	*/
#define ZC_B_DDR		DDRB
#define ZC_B_PORT		PORTB
#define ZC_B_PINS		PINB
#define ZC_B_PIN		PINB0
#define ZC_B_INT		PCINT0

/*	Pin used to detect zero crossing for phase C. PCINT11.	*/
#define ZC_C_DDR		DDRC
#define ZC_C_PORT		PORTC
#define ZC_C_PINS		PINC
#define ZC_C_PIN		PINC3
#define ZC_C_INT		PCINT11

extern const uint8_t driver_states[6];

void InitIO(void);
void InitTimers(void);
void InitInterrupts(void);