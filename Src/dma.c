#include "main.h"

void init_dma(void)
{
	// enable the clk for the dmamux modul
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	
	// enabled the clk for the dma modul
	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;
	
	// enable the channel 0 and select tpm0 channel 1 as source (ref manual page 68)
	DMAMUX0_CHCFG0 |= DMAMUX_CHCFG_ENBL_MASK | DMAMUX_CHCFG_SOURCE(25);
	
	// enable request signal for channel 0 
	DMA_DCR0 |= DMA_DCR_ERQ_MASK;
	
	
	
	
}
