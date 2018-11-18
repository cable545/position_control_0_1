#include "main.h"

static volatile unsigned int DelayTimerTick = 0;

volatile unsigned int Ticker[4];

/*
 *	User manual Section 4.4! 
 *	Every ms comes an Interrupt => 48000000 / 1000 = 48000
 */
void InitSysTick(void)
{
	SYST_RVR = CORE_CLOCK/SYSTICK_FREQUENCY; // Set the Reload to the interrupt at our desired frequency
	SYST_CVR = 0; // Reset the Current Value
	SYST_CSR = SysTick_CSR_ENABLE_MASK | SysTick_CSR_TICKINT_MASK | SysTick_CSR_CLKSOURCE_MASK;

	enable_irq(INT_SysTick -16);
	set_irq_priority((INT_SysTick -16), 2);
}

/*
 * every 3.3 ms comes an Interrupt 
 * => 1ms 48000000 / 1000 = 48000 ticks, 0.1ms 48000000 / 10000 = 4800 ticks, 4800 * 3 + 48000 * 3 = 158400
 */
void InitSysTickForSensorFusion(void)
{
//	SYST_RVR = CORE_CLOCK/SENSOR_READ_FREQUENCY; // Set the Reload to the interrupt at our desired frequency
	SYST_RVR = SENSOR_READ_TICKS;
	SYST_CVR = 0; // Reset the Current Value
	SYST_CSR = SysTick_CSR_ENABLE_MASK | SysTick_CSR_TICKINT_MASK | SysTick_CSR_CLKSOURCE_MASK;

	enable_irq(INT_SysTick -16);
	set_irq_priority((INT_SysTick -16), 2);
}

void SysTick_Handler() 
{
//	unsigned int i;
	
	if(DelayTimerTick<0xFFFFFFFF)
	{
		DelayTimerTick++;
	}
	
//	for(i=0;i<4;i++)
//	{
//		if(Ticker[i]<0xFFFFFFFF)
//			{
//				Ticker[i]++;
//			}
//	}
}

void Delay_mS(unsigned int TicksIn_mS)
{
	DelayTimerTick = 0;

	while(DelayTimerTick < TicksIn_mS)
	{
	}
}

