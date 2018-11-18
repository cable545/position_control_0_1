#include <stdio.h>
#include "common.h"
#include "switches.h"
#include "interrupt.h"
volatile uint32_t ownTicker = 0;

//void PORTC_PORTD_IRQHandler()
//{
//	if(PORTC_PCR3 & (1<<24))
//	{
//		PORTC_PCR3 |= (1<<24);
//		
//		printf("SWITCH 1 wurde gedrueckt\n");
//	}
//	
//	if(PORTC_PCR12 & (1<<24))
//	{
//		PORTC_PCR12 |= (1<<24);
//		
//		printf("SWITCH 3 wurde gedrueckt\n");
//	}
//}

void initBoardSwitch(int switch_mask, int interrupt)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;													//This enables the clock for port c.
	
	if(switch_mask == SWITCH_1)
	{
		//This sets the Mux control of PORTC_PCR3 to GPIO and set the interrupt
		PORTC_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(interrupt) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;	
		
		//This sets GPIOC PORT3 as an input.  
		GPIOC_PDDR &= ~(1<<SWITCH_1_PIN); 													
	}
	else if(switch_mask == SWITCH_3)
	{
		//This sets the Mux control of PORTC_PCR12 to GPIO and set the interrupt
		PORTC_PCR12 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(interrupt) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; 
		
		GPIOC_PDDR |= (0<<SWITCH_3_PIN); 													//This sets GPIOC PORT12 as an input.  
	}
	else if(switch_mask == (SWITCH_1 | SWITCH_3))
	{
		//This sets the Mux control of PORTC_PCR3 to GPIO and set the interrupt
		PORTC_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(interrupt) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;	
		//This sets the Mux control of PORTC_PCR12 to GPIO and set the interrupt
		PORTC_PCR12 = PORT_PCR_MUX(1) | PORT_PCR_IRQC(interrupt) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; 
		
		GPIOC_PDDR |= (0<<SWITCH_1_PIN) | (0<<SWITCH_3_PIN); 			//This sets GPIOC PORT3 and PORT12 as an input.  
	}
	
	if(interrupt != 0)
		enable_irq(INT_PORTD - 16);
}
