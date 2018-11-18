#ifndef SWITCHES_H
#define	SWITCHES_H

#define SWITCH_1 (1<<3)
#define SWITCH_3 (1<<12)

#define SWITCH_1_PIN 3
#define SWITCH_3_PIN 12

#define INTERRUPT_DISABLED 0	//Interrupt disabled
#define LOGIC_ZERO		8				//Interrupt when logic zero
#define RISING_EDGE 	9 			//Interrupt on rising edge
#define FALLING_EDGE 	10 			//Interrupt on falling edge
#define EITHER_EDGE		11			//Interrupt on either edge
#define LOGIC_ONE			12			//Interrupt when logic one
extern volatile uint32_t ownTicker;
void initBoardSwitch(int switch_mask, int interrupt);

#endif

