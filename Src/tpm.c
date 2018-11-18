#include "main.h"

extern int mcg_clk_hz;

int tpm_init(TPM_MemMapPtr TPMx, int  clock_source, int modulo, int clock_mode, int ps, int toi_enable, int set_ca_pwm)
{
	int val = 1;
	int i;
	
	// enables the clock for one of the tpm modules
	if(TPMx == TPM0_BASE_PTR)
		SIM_SCGC6	|= SIM_SCGC6_TPM0_MASK;
	else if(TPMx == TPM1_BASE_PTR)
		SIM_SCGC6	|= SIM_SCGC6_TPM1_MASK;
	else
		SIM_SCGC6	|= SIM_SCGC6_TPM2_MASK;
	
	// sets the clock source for the tpm modul
	SIM_SOPT2	|= SIM_SOPT2_TPMSRC(clock_source);
	
	TPM_MOD_REG(TPMx) =  modulo;
	
	// activates the center-aligned pwm if set_ca_pwm equals TRUE
	if(set_ca_pwm)
		TPM_SC_REG(TPMx) |= TPM_SC_CPWMS_MASK;
	
	// enables the counter, enables the timer overflow interrupt and sets the clockmode and the prescaler
	if(toi_enable)
		TPM_SC_REG(TPMx)	= TPM_SC_TOIE_MASK | TPM_SC_CMOD(clock_mode) | TPM_SC_PS(ps);
	else
		TPM_SC_REG(TPMx)	= TPM_SC_CMOD(clock_mode) | TPM_SC_PS(ps);
	
	i= ps;
	
	while(i--)
	{
		val += val;
	}
	
	val=(mcg_clk_hz/2)/val;
	
	return val;
}

void tpm_clk_mode(TPM_MemMapPtr TPMx, int clock_mode)
{
	int temp;
	temp = TPM_SC_REG(TPMx);
	temp &= ~TPM_SC_CMOD(3);
	temp |= TPM_SC_CMOD(clock_mode);
	TPM_SC_REG(TPMx) =	temp;
}

void tpm_clear_counter(TPM_MemMapPtr TPMx)
{
	TPM_CNT_REG(TPMx) = 0;
}

uint16 tpm_get_cnt_value(TPM_MemMapPtr TPMx)
{
	return TPM_CNT_REG(TPMx);
}

void tpm_ch_init(TPM_MemMapPtr TPMx, int channel, int mode, int interrupt_on, int dma_on)
{
	if(interrupt_on && dma_on)
		TPM_CnSC_REG(TPMx, channel) = mode | TPM_CnSC_CHIE_MASK | TPM_CnSC_DMA_MASK;
	else if(interrupt_on)
		TPM_CnSC_REG(TPMx, channel) = mode | TPM_CnSC_CHIE_MASK;
	else if(dma_on)
		TPM_CnSC_REG(TPMx, channel) = mode | TPM_CnSC_DMA_MASK;
	else
		TPM_CnSC_REG(TPMx, channel) = mode;
}

void tpm_set_CnV(TPM_MemMapPtr TPMx, int channel, int value)
{
	TPM_CnV_REG(TPMx, channel) = value;
}
