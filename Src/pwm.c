#include "main.h"

void init_pwm_for_motors(void)
{
	PORTA_PCR4 = PORT_PCR_MUX(PIN_ALT_3) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTA_PCR5 = PORT_PCR_MUX(PIN_ALT_3) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTE_PCR30 = PORT_PCR_MUX(PIN_ALT_3) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	PORTE_PCR31 = PORT_PCR_MUX(PIN_ALT_3) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	
	tpm_init(TPM0_BASE_PTR, TPM_PLLFLL, PWM_2_0_MS, TPM_CLK, PS_16, TPM_TOINT_DIS, FALSE);
	
  tpm_ch_init(TPM0_BASE_PTR, MOTOR_A, TPM_EA_PWM_H, FALSE, FALSE);
	tpm_ch_init(TPM0_BASE_PTR, MOTOR_B, TPM_EA_PWM_H, FALSE, FALSE);
	tpm_ch_init(TPM0_BASE_PTR, MOTOR_C, TPM_EA_PWM_H, FALSE, FALSE);
	tpm_ch_init(TPM0_BASE_PTR, MOTOR_D, TPM_EA_PWM_H, FALSE, FALSE);
	
	tpm_set_CnV(TPM0_BASE_PTR, MOTOR_A, PWM_MIN);
	tpm_set_CnV(TPM0_BASE_PTR, MOTOR_B, PWM_MIN);
	tpm_set_CnV(TPM0_BASE_PTR, MOTOR_C, PWM_MIN);
	tpm_set_CnV(TPM0_BASE_PTR, MOTOR_D, PWM_MIN);
}

void set_motor_throttle(int value, int motor)
{	
	tpm_set_CnV(TPM0_BASE_PTR, motor, value);
}
