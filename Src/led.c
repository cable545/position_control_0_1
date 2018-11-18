#include "main.h"

//void initInLED(void)
//{
//	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
//	PORTA_PCR4 = PORT_PCR_MUX(1); 	
//	GPIOA_PSOR |= 1<<4; 																		//Set GPIOD_PSOR Port 5 to logic 1 as initial value
//	GPIOA_PDDR |= 1<<4; 	
//	GPIOA_PCOR  = 1<<4; //set the led
//}

//This function enables the red or green or both LEDs on the FRDM-KL46Z development board
void initBoardLED(int led_mask)
{ 
	if(led_mask == RED_OB_LED)
	{
		PORTE_PCR29 = PORT_PCR_MUX(1); 														//This sets the Mux control of PORTD_PCR5 to GPIO
		GPIOE_PSOR |= RED_OB_LED; 																		//Set GPIOD_PSOR Port 5 to logic 1 as initial value
		GPIOE_PDDR |= RED_OB_LED; 																		//This sets GPIOD PORT5 as an output.  
	}
	else if(led_mask == GREEN_OB_LED)
	{
		PORTD_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;					//This sets the Mux control of PORTE_PCR29 to GPIO
		GPIOD_PSOR |= GREEN_OB_LED;																	//Set GPIOD_PSOR Port 29 to logic 1 as initial value
		GPIOD_PDDR |= GREEN_OB_LED;																	//This sets GPIOD PORT29 as an output.  
	}		
	else if(led_mask == (RED_OB_LED | GREEN_OB_LED))
	{
		PORTE_PCR29 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK;				//This sets the Mux control of PORTE_PCR29 to GPIO
		PORTD_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_DSE_MASK; 				//This sets the Mux control of PORTD_PCR5 to GPIO
		GPIOE_PSOR |= RED_OB_LED; 																		//Set GPIOD_PSOR Port 5 to logic 1 as initial value
		GPIOD_PSOR |= GREEN_OB_LED;																	//Set GPIOD_PSOR Port 29 to logic 1 as initial value
		GPIOE_PDDR |= RED_OB_LED; 																		//This sets GPIOD PORT5 as an output.  
		GPIOD_PDDR |= GREEN_OB_LED;																	//This sets GPIOD PORT29 as an output.  
	}
	else
	{
		//TODO PRINT ERROR
	}
}

void tooglesLED(int led_mask)
{
	if(led_mask == GREEN_OB_LED)
	{
		GPIOD_PTOR = GREEN_OB_LED; 																	//Toggles GPIOD Port 5(GREEN_OB_LED)
	}
	else if(led_mask == RED_OB_LED)
	{
		GPIOE_PTOR = RED_OB_LED; 																		//Toggles GPIOE Port 29(RED_OB_LED)
	}		
	else if(led_mask == (GREEN_OB_LED | RED_OB_LED))
	{
		GPIOD_PTOR = GREEN_OB_LED; 																	//Toggles GPIOD Port 5(GREEN_OB_LED) with Port-Toogle-Output Register
		GPIOE_PTOR = RED_OB_LED; 																		//Toggles GPIOE Port 29(RED_OB_LED) with Port-Toogle-Output Register
	}
	else
	{
		//TODO PRINT ERROR
	}
}

void setLED(int led_mask)
{
	if(led_mask == GREEN_OB_LED)
	{
		GPIOD_PCOR  = GREEN_OB_LED; 																	//Set GPIOD_PSOR Port 5 to logic 0, turns GREEN_OB_LED on
	}
	else if(led_mask == RED_OB_LED)
	{
		GPIOE_PCOR  = RED_OB_LED; 																		//Set GPIOE_PSOR Port 29 to logic 0, turns RED_OB_LED on
	}		
	else if(led_mask == (GREEN_OB_LED | RED_OB_LED))
	{
		GPIOD_PCOR  = GREEN_OB_LED; 																	//Set GPIOD_PSOR Port 5 to logic 0, turns GREEN_OB_LED on
		GPIOE_PCOR  = RED_OB_LED; 																		//Set GPIOE_PSOR Port 29 to logic 0, turns RED_OB_LED on
	}
	else
	{
		//TODO PRINT ERROR
	}
}

void clearLED(int led_mask)
{
	if(led_mask == GREEN_OB_LED)
	{
		GPIOD_PSOR = GREEN_OB_LED; 																	//Set GPIOD_PSOR Port 5 to logic 1, turns GREEN_OB_LED off
	}
	else if(led_mask == RED_OB_LED)
	{
		GPIOE_PSOR = RED_OB_LED; 																		//Set GPIOE_PSOR Port 29 to logic 1, turns RED_OB_LED off
	}		
	else if(led_mask == (GREEN_OB_LED | RED_OB_LED))
	{
		GPIOD_PSOR = GREEN_OB_LED; 																	//Set GPIOD_PSOR Port 5 to logic 1, turns GREEN_OB_LED off
		GPIOE_PSOR = RED_OB_LED; 																		//Set GPIOE_PSOR Port 29 to logic 1, turns RED_OB_LED off
	}
	else
	{
		//TODO PRINT ERROR
	}
}

void init_rled_pwm(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;												//This enables the clock for port e.
	PORTE_PCR29 = PORT_PCR_MUX(3); 														//This sets the Mux control of PORTE_PCR29 to TPM0_CH2
}

void init_testled_pwm(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;												//This enables the clock for port e.
	PORTA_PCR4 = PORT_PCR_MUX(3); 														//This sets the Mux control of PORTA_PCR4 to TPM0_CH1
}

