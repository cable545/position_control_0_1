#include "main.h"

void init_buzzer()
{
	PORTE_PCR21 = PORT_PCR_MUX(1); 														//This sets the Mux control of PORTE_PCR21 to GPIO
	GPIOE_PSOR |= (1<<21); 																		//Set GPIOD_PSOR Port 5 to logic 1 as initial value
	GPIOE_PDDR |= (1<<21);
}

// creates a one second long beep
void beep()
{
	GPIOE_PSOR = (1<<21);
	for(int i = 7800; i != 0; i--);
	GPIOE_PCOR = (1<<21);
	for(int i = 7800; i != 0; i--);
}

void beeps(int count)
{
	for(int c = count; c != 0; c--)
	{
		for(int i = 500; i != 0; i--)
			beep();
		
		for(int j = 4000000; j != 0; j--);
	}
}
