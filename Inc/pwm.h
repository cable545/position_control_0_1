#ifndef PWM_H
#define PWM_H

#define PWM_LIMIT		2000

#define PWM_1_0_MS	1500
#define PWM_1_5_MS	2250
#define PWM_2_0_MS	3000

#define PWM_MIN PWM_1_0_MS
#define PWM_MAX PWM_1_5_MS

#define MOTOR_A TPM_CHANNEL_1
#define MOTOR_B TPM_CHANNEL_3
#define MOTOR_C TPM_CHANNEL_4
#define MOTOR_D TPM_CHANNEL_2


void init_pwm_for_motors(void);
void set_motor_throttle(int value, int motor);

#endif
