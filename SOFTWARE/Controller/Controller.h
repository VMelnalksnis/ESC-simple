/*
 * Controller.h
 *
 * Created: 01.05.2018 15:47:08
 *  Author: Valters Melnalksnis
 */

/*	Pin map V2A
 *	12	PB0		ICP		IN	Control signal
 *	13	PB1		PCINT0	IN	Zero crossing Phase B (ZC_B)
 *	14	PB2		-		-	N/C
 *	15	PB3		MOSI	-	ISP
 *	16	PB4		MISO	-	ISP
 *	17	PB5		SCK		-	ISP
 *	7	PB6		XTAL1	-	Crystal
 *	8	PB7		XTAL2	-	Crystal
 *
 *	23	PC0		-		OUT	Test/status LED 1
 *	24	PC1		-		OUT	Test/status LED 2
 *	25	PC2		-		-	N/C
 *	26	PC3		PCINT11	IN	Zero crossing Phase C (ZC_C)
 *	27	PC4		-		-	N/C
 *	28	PC5		-		-	N/C
 *	29	PC6		RESET	-	ISP
 *
 *	30	PD0		-		OUT	Shutdown signal Phase C (SD_C)
 *	31	PD1		-		OUT	Phase signal Phase C (PHASE_C)
 *	32	PD2		-		-	N/C
 *	1	PD3		-		OUT	Shutdown signal Phase B (SD_B)
 *	2	PD4		-		OUT	Phase signal Phase B (PHASE_B)
 *	9	PD5		-		OUT Shutdown signal Phase A (SD_A)
 *	10	PD6		-		OUT	Phase signal Phase A (PHASE_A)
 *	11	PD7		PCINT23	IN	Zero crossing Phase A (ZC_A)
 *
 *	19	ADC6	-		-	N/C
 *	22	ADC7	ADC		IN	Current measurement
 */

#include <avr/io.h>
#include <avr/interrupt.h>

/*	Pins connected to LEDs used for testing and status indication.	*/
#define TEST_PIN_1		PINC0
#define TEST_PIN_2		PINC1
/*	DDR associated with the test/status pins.*/
#define TEST_DDR		DDRC
/*	PORT register associated with the test/status pins.	*/
#define TEST_PORT		PORTC

/*	Pins used to control the 3 half-bridge drivers.	*/
#define SD_C			PIND0
#define PHASE_C			PIND1
#define SD_B			PIND3
#define PHASE_B			PIND4
#define SD_A			PIND5
#define PHASE_A			PIND6
/*	DDR associated with the driver control pins.	*/
#define DRIVER_DDR		DDRD
/*	PORT register associated with the driver control pins.	*/
#define DRIVER_PORT		PORTD

/*	DRIVER_PORT states during each commutation state (ON).	*/
#define DRIVER_STATE_ON_1	((1 << SD_A) | (1 << PHASE_A) | (1 << SD_B))
#define DRIVER_STATE_ON_2	((1 << SD_A) | (1 << PHASE_A) | (1 << SD_C))
#define DRIVER_STATE_ON_3	((1 << SD_B) | (1 << PHASE_B) | (1 << SD_C))
#define DRIVER_STATE_ON_4	((1 << SD_B) | (1 << PHASE_B) | (1 << SD_A))
#define DRIVER_STATE_ON_5	((1 << SD_C) | (1 << PHASE_C) | (1 << SD_A))
#define DRIVER_STATE_ON_6	((1 << SD_C) | (1 << PHASE_C) | (1 << SD_B))

/*	DRIVER_PORT states during each commutation state (OFF).	*/
#define DRIVER_STATE_OFF_1	((1 << SD_A) | (1 << PHASE_A) | (1 << SD_B) | (1 << PHASE_B))
#define DRIVER_STATE_OFF_2	((1 << SD_A) | (1 << SD_C))
#define DRIVER_STATE_OFF_3	((1 << SD_B) | (1 << PHASE_B) | (1 << SD_C) | (1 << PHASE_C))
#define DRIVER_STATE_OFF_4	((1 << SD_B) | (1 << SD_A))
#define DRIVER_STATE_OFF_5	((1 << SD_C) | (1 << PHASE_C) | (1 << SD_A) | (1 << PHASE_A))
#define DRIVER_STATE_OFF_6	((1 << SD_C) | (1 << SD_B))

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

/*	Minimum control signal pulse width (1ms).	*/
#define CTRL_WIDTH_MIN	16000
/*	Maximum control signal pulse width (2ms).	*/
#define CTRL_WIDTH_MAX	32000

/*	Array containing driver commutation states in order.	*/
extern const uint8_t driver_states_on[6];
extern const uint8_t driver_states_off[6];

void InitIO(void);
void BlindStart(void);