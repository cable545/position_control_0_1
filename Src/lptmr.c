#include "main.h"

void lptmr_init(int count, int erclk32k_source, int clock_source, int prescaler)
{
	// enables the access to the lptmr
	SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;
	SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(erclk32k_source); // System oscillator (OSC32KCLK) is preferred
	
	LPTMR0_PSR = (
		LPTMR_PSR_PRESCALE(prescaler) // 0000 is div 2
		| LPTMR_PSR_PBYP_MASK  				// LPO feeds directly to LPT
		| LPTMR_PSR_PCS(clock_source) // use the choice of clock
	);
           
	LPTMR0_CMR = LPTMR_CMR_COMPARE(count);  //Set compare value
	
	LPTMR0_CSR =(
		LPTMR_CSR_TCF_MASK   		// Clear any pending interrupt
		| !LPTMR_CSR_TIE_MASK   // LPT interrupt enabled
		| LPTMR_CSR_TPS(0)     	//TMR pin select
		|!LPTMR_CSR_TPP_MASK   	//TMR Pin polarity
		|!LPTMR_CSR_TFC_MASK   	// Timer Free running counter is reset whenever TMR counter equals compare
		|!LPTMR_CSR_TMS_MASK   	//LPTMR0 as Timer
	);
  
	LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   //Turn on LPT and start counting
}
