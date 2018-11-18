#include "common.h"
#include "interrupt.h"
#include "TPM.h"

//CPWMS = 0
//MSnB:MSnA = 1:0

//EPWM period = MOD + 0x0001
//pulse width (duty cycle) = CnV

volatile uint32_t TPM0_Ticker = 0;

void TPM0_IRQHandler()
{
    //Clear the overflow mask if set.   According to the reference manual, we clear by writing a logic one!
		if(TPM0_SC & TPM_SC_TOF_MASK)
		{
			TPM0_SC |= TPM_SC_TOF_MASK;
		}
		
		if(TPM0_Ticker<0xFFFFFFFF)
			TPM0_Ticker++;
}

/*
 *	TPMx: basepointer from the the tpm modul
 *	
 *
 */
void initTPM(TPM_MemMapPtr TPMx, int clock_source, int fll_pll, int modulo, int clock_mode, int prescaler, int enableInterrupt)
{
	// set the clock for the TPM
	SIM_SOPT2 &= ~(SIM_SOPT2_TPMSRC_MASK); 						// reset bit 24 and 25, clock source for TPM
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(clock_source);			// TPM source is MCGFLLCLK clock or MCGPLLCLK/2   
	if(clock_source && fll_pll)												// if clk_source PLL
		SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; 					// MCGPLLCLK clock with fixed divide by two
	
	// enable the clock to the TPMx 
	if(TPMx == TPM0_BASE_PTR)
		SIM_SCGC6	|= SIM_SCGC6_TPM0_MASK;
	else if(TPMx == TPM1_BASE_PTR)
		SIM_SCGC6	|= SIM_SCGC6_TPM1_MASK;
	else
		SIM_SCGC6	|= SIM_SCGC6_TPM2_MASK;
	
	//setup the TPM peripheral directly
	TPM0_SC = 0x0; // disables the TPM counter
	TPM0_CONF = 0;

	// Enable the TPM Counter(clock mode) and set the prescaler
	TPM_SC_REG(TPMx) |= TPM_SC_CMOD(clock_mode) | TPM_SC_PS(prescaler);
	
	//TPM0_MOD = FTM0_CLOCK/(1 << FTM0_CLK_PRESCALE)/PWM_FREQUENCY;
	TPM_MOD_REG(TPMx) = modulo;
               
	// if interrupt = 1 enable the interrupt for timer overflow in tpm and NVIC
	if(enableInterrupt)
	{
		TPM_SC_REG(TPMx) |= TPM_SC_TOIE_MASK;
		//Enable TPM1 IRQ on the NVIC
		enable_irq (INT_TPM0-16);
	}
}

void TPM_init_PWM(TPM_MemMapPtr TPMx, int  clock_source, int fll_pll, int modulo, int clock_mode, int prescaler, int counting_mode)
{
	SIM_SOPT2 &= ~(SIM_SOPT2_TPMSRC_MASK); 					// reset bit 24 and 25, clock source for TPM
	SIM_SOPT2	|= SIM_SOPT2_TPMSRC(clock_source); 		// TPM source is MCGFLLCLK clock or MCGPLLCLK/2
	if(clock_source && fll_pll)											// if clk_source PLL
		SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; 				// MCGPLLCLK clock with fixed divide by two
	
	// enable the clock to the TPMx 
	if(TPMx == TPM0_BASE_PTR)
		SIM_SCGC6	|= SIM_SCGC6_TPM0_MASK;
	else if(TPMx == TPM1_BASE_PTR)
		SIM_SCGC6	|= SIM_SCGC6_TPM1_MASK;
	else
		SIM_SCGC6	|= SIM_SCGC6_TPM2_MASK;
	
	TPM_MOD_REG(TPMx) = modulo;
	
	// Enable the TPM Counter(clock mode) and set the prescaler
	TPM_SC_REG(TPMx)	|= TPM_SC_CMOD(clock_mode) | TPM_SC_PS(prescaler);
	
	// if 
	if(counting_mode)
		TPM_SC_REG(TPMx) |= TPM_SC_CPWMS_MASK;
}

void TPM_CH_init_PWM(TPM_MemMapPtr TPMx, int channel, int mode)
{
	TPM_CnSC_REG(TPMx, channel) |= mode;
}

void set_TPM_CnV_PWM(TPM_MemMapPtr TPMx, int channel, int value)
{
	TPM_CnV_REG(TPMx, channel) = value;
}
