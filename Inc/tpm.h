#ifndef TPM_H
#define TPM_H

#define TPM_MAX_MODULE	0xFFFF

#define TPM_TOINT_DIS	0
#define TPM_TOINT_EN  1

// TPM clock sources
#define TPM_PLLFLL 		1	// TPM source is MCGFLLCLK clock or MCGPLLCLK/2
#define TPM_OSCERCLK 	2
#define TPM_MCGIRCLK	3

// TPM clock modes (manual page 567)
#define	TPM_CNT_DIS		0
#define	TPM_CLK				1
#define	TPM_EXT_CLK		2

// prescaler constants
#define PS_1					0
#define PS_2					1
#define PS_4					2
#define PS_8					3
#define PS_16					4
#define PS_32					5
#define PS_64					6
#define PS_128				7

// constants for tpm channels
#define TPM_CHANNEL_0			0
#define TPM_CHANNEL_1			1
#define TPM_CHANNEL_2			2
#define TPM_CHANNEL_3			3
#define TPM_CHANNEL_4			4
#define TPM_CHANNEL_5			5

// channel modes(input capture, output compare, center-aligned pwm, edge-aligned pwm)
#define TPM_IC_RISING		TPM_CnSC_ELSA_MASK
#define TPM_IC_FALLING	TPM_CnSC_ELSB_MASK
#define TPM_IC_BOTH			TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK
#define TPM_EA_PWM			
#define TPM_CA_PWM

#define TPM_OC_TOGGLE	TPM_CnSC_MSA_MASK|TPM_CnSC_ELSA_MASK
#define TPM_OC_CLR		TPM_CnSC_MSA_MASK|TPM_CnSC_ELSB_MASK
#define TPM_OC_SET		TPM_CnSC_MSA_MASK|TPM_CnSC_ELSA_MASK|TPM_CnSC_ELSB_MASK
#define TPM_OC_OUTL		TPM_CnSC_MSB_MASK|TPM_CnSC_MSA_MASK|TPM_CnSC_ELSB_MASK
#define TPM_OC_OUTH		TPM_CnSC_MSB_MASK|TPM_CnSC_MSA_MASK|TPM_CnSC_ELSA_MASK

#define TPM_EA_PWM_H 		TPM_CnSC_MSB_MASK|TPM_CnSC_ELSB_MASK
#define TPM_EA_PWM_L		TPM_CnSC_MSB_MASK|TPM_CnSC_ELSA_MASK


int tpm_init(TPM_MemMapPtr TPMx, int  clock_source, int modulo, int clock_mode, int ps, int toi_enable, int set_ca_pwm);
void tpm_clk_mode(TPM_MemMapPtr TPMx, int clock_mode);
void tpm_clear_counter(TPM_MemMapPtr TPMx);
uint16 tpm_get_cnt_value(TPM_MemMapPtr TPMx);
void tpm_ch_init(TPM_MemMapPtr TPMx, int channel, int mode, int interrupt_on, int dma_on);
void tpm_set_CnV(TPM_MemMapPtr TPMx, int channel, int value);

#endif
