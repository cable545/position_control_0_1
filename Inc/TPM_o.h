#ifndef _TPM_H_
#define _TPM_H_

#define FTM0_CLK_PRESCALE 			6					// means a prescaler value of 64
#define FTM0_CLOCK 							24000000
#define FTM0_OVERFLOW_FREQUENCY 50 
#define PWM_FREQUENCY 					100  // Hz

#define TPM_PLLFLL		1
#define TPM_OSCERCLK 	2
#define TPM_MCGIRCLK	3

#define TPM_FLL	0
#define TPM_PLL 1

#define	TPM_CNT_DIS	0
#define	TPM_CLK			1
#define	TPM_EXT_CLK	2

#define PS_1		0
#define PS_2		1
#define PS_4		2
#define PS_8		3
#define PS_16		4
#define PS_32		5
#define PS_64		6
#define PS_128	7

#define TPM_OC_TOGGLE	TPM_CnSC_MSA_MASK|TPM_CnSC_ELSA_MASK
#define TPM_OC_CLR		TPM_CnSC_MSA_MASK|TPM_CnSC_ELSB_MASK
#define TPM_OC_SET		TPM_CnSC_MSA_MASK|TPM_CnSC_ELSA_MASK|TPM_CnSC_ELSB_MASK
#define TPM_OC_OUTL		TPM_CnSC_MSB_MASK|TPM_CnSC_MSA_MASK|TPM_CnSC_ELSB_MASK
#define TPM_OC_OUTH		TPM_CnSC_MSB_MASK|TPM_CnSC_MSA_MASK|TPM_CnSC_ELSA_MASK

#define TPM_PWM_H 		TPM_CnSC_MSB_MASK|TPM_CnSC_ELSB_MASK
#define TPM_PWM_L		TPM_CnSC_MSB_MASK|TPM_CnSC_ELSA_MASK

#define EDGE_PWM			0	// edge aligned pwm
#define CENTER_PWM		1 // center aligned pwm

extern volatile uint32_t TPM0_Ticker;
void initTPM0(TPM_MemMapPtr TPMx, int clock_source, int fll_pll, int modulo, int clock_mode, int prescaler, int enableInterrupt);

void TPM_init_PWM(TPM_MemMapPtr TPMx, int  clock_source, int real_clk_source, int modulo, int clock_mode, int prescaler, int counting_mode);
void TPM_CH_init_PWM(TPM_MemMapPtr TPMx, int channel, int mode);
void set_TPM_CnV_PWM(TPM_MemMapPtr TPMx, int channel, int value);

#endif
