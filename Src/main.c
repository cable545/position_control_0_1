#include "main.h"

main_loop_states state = init_hardware_state;

float DELTA_T = 0.0;

static uint32 tof_index = 0;

extern unsigned short sum_signal_array[MAX_CHANNELS];
extern int roll, pitch, yaw;
extern Imu_Data acc_values, gyro_values, mag_values;
volatile extern int receiver_ready;

extern int angle_controller_activated;

/*----------------------------------------------------------------------------------------------*/
/*                                            main function                                     */
/*----------------------------------------------------------------------------------------------*/
int main(void)
{
	// initialize the hardware
	if(sysinit())
		state = init_software_state;
	else
		state = error_state;
	
	if(state == init_software_state)
	{
		print(EXT_DEBUG_HANDLE, "initializing software\r\n");
		
		// inits the kalman structures
		kalman_init_all_angles();
		
		// inits the receiver for signal processing
		init_receiver(TRUE, FALSE);
		// wait until receiver synchronisation is ready
		while(receiver_ready == FALSE);
		
		// inits the pwm for the control signals for the motors
		init_pwm_for_motors();
		
		state = security_state;
		
		print(EXT_DEBUG_HANDLE, "starting software\r\n");
		
		for(;;)
		{
			go_in_sec_state();
			
			while(state == security_state)
			{
				read_sensor_data();
				
				#ifdef BLUETOOTH_ON
				compute_bluetooth_data();
				send_debug_data(NULL_VALUE);
				#endif
				
				for(int i = 1250000; i != 0; i--);
				tooglesLED(RED_OB_LED);
				check_sum_signal_values();
			}
			
			go_in_op_state();
			
			while(state == operating_state)
			{
				start_timer();
				
				check_sum_signal_values();
				evaluate_rc_switch_values();
				
				#ifdef BLUETOOTH_ON
				send_debug_data(NULL_VALUE);
				#endif
				
				read_sensor_data();
				sensor_fusion();
				controller();
				
				DELTA_T = stop_timer() / 1000;
			}
		}
	}
	else
	{
		clearLED(GREEN_OB_LED);
		setLED(RED_OB_LED);
		
		for(;;)
		{
			for(int i = 3000000; i != 0; i--);
			tooglesLED(GREEN_OB_LED);
			tooglesLED(RED_OB_LED);
			beeps(1);
		}
	}
		
	return 0;
}



/*----------------------------------------------------------------------------------------------*/
/*                              timer functions for time measurements                           */
/*----------------------------------------------------------------------------------------------*/
void init_timer(void)
{
	// initialze the tpm 2 modul
	tpm_init(TPM2_BASE_PTR, TPM_PLLFLL, TPM_MAX_MODULE, TPM_CLK, PS_16, TPM_TOINT_EN, FALSE);
}

void start_timer(void)
{
	// enable tmp2 interrupts
	enable_irq(INT_TPM2 -16);
	set_irq_priority((INT_TPM2 -16), 3);
	
	tof_index = 0;
	tpm_clear_counter(TPM2_BASE_PTR);
}

float get_millis(void)
{
	return ((tof_index * TPM_MAX_MODULE) + tpm_get_cnt_value(TPM2_BASE_PTR)) / 1500.0;
}

float stop_timer(void)
{
	// disable tmp2 interrupts
	disable_irq(INT_TPM2 -16);
	
	return ((tof_index * TPM_MAX_MODULE) + tpm_get_cnt_value(TPM2_BASE_PTR)) / 1500.0;
}

void TPM2_IRQHandler(void)
{
	if(TPM2_SC & TPM_SC_TOF_MASK)
	{
		TPM2_SC |= TPM_SC_TOF_MASK;
		
		tof_index++;
	}
}

int map(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void go_in_sec_state(void)
{
	beeps(2);
	for(int i = 3000000; i != 0; i--);
	clearLED(GREEN_OB_LED);
	setLED(RED_OB_LED);
}

void go_in_op_state(void)
{
	beeps(3);
	for(int i = 3000000; i != 0; i--);
	clearLED(RED_OB_LED);
	setLED(GREEN_OB_LED);
}

