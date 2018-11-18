#include "main.h"

int receiver_ready = FALSE;
int kalman_activated = TRUE;
int angle_controller_activated = TRUE;

unsigned short sum_signal_array[MAX_CHANNELS];

static unsigned char sum_signal_index = 0;
static short last_valid_values[MAX_CHANNELS];

extern main_loop_states state;

extern int bluetooth_receiver_active;

/*
 *	interrupt handler for the tpm1 modul implementation for the sum signal processing
 */
void TPM1_IRQHandler(void)
{
	if(TPM1_C0SC & TPM_CnSC_CHF_MASK)
	{
		// clears the event flag
		TPM1_C0SC |= TPM_CnSC_CHF_MASK;
		
		/*
		 *	measuring error course a wrong rising edge
		 *	leave the interrupt handler without a timer reset
		 */
		if(TPM1_C0V < CHANNEL_VALUE_COUNT_MIN - 500)
			return;
		
		if(TPM1_C0V >= COUNT_BORDER_SYNC)
		{
			receiver_ready = TRUE;
			sum_signal_index = 0;	
		}
		else if(TPM1_C0V < COUNT_BORDER_SYNC)
		{
			sum_signal_array[sum_signal_index] = TPM1_C0V;
			sum_signal_index++;
		}
		else
		{}
			
		// clears the CNT register, timer reset
		TPM1_CNT = 0xff;
	}
}

/*
 *	initialize the ports, interrupt, dma and the tpm modul for the sum signal processing
 */
void init_receiver(int set_interrupt, int set_dma)
{
	sum_signal_array[0] = CHANNEL_VALUE_COUNT_MIN;
	sum_signal_array[1] = CHANNEL_VALUE_MIDDLE;
	sum_signal_array[2] = CHANNEL_VALUE_MIDDLE;
	sum_signal_array[3] = CHANNEL_VALUE_MIDDLE;
	sum_signal_array[4] = CHANNEL_VALUE_COUNT_MIN;
	sum_signal_array[5] = CHANNEL_VALUE_COUNT_MIN;
	
	// sets the Mux control of PORTE_PCR20 to TPM1_CH0 and enables the internal pullup resistor
	PORTE_PCR20 = PORT_PCR_MUX(PIN_ALT_3) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
	
	if(set_interrupt)
	{
		// enable tmp0 interrupts
		enable_irq(INT_TPM1 -16);
		set_irq_priority((INT_TPM1 -16), 1);
	}
	
	// initialize the tpm 1 modul
	tpm_init(TPM1_BASE_PTR, TPM_PLLFLL, TPM_MAX_MODULE, TPM_CLK, PS_16, TPM_TOINT_DIS, FALSE);

	// initialize the channel 0 of the tpm1 module
	tpm_ch_init(TPM1_BASE_PTR, TPM_CHANNEL_0, TPM_IC_FALLING, set_interrupt, set_dma);
}

/*
 *	calculates the corresponding counts from an angle
 */
int get_counts_from_angle(int* angle)
{
	int counts;
	
	if(*angle == 0)
		return CHANNEL_VALUE_MIDDLE;
	
	counts = (int)(*angle / DEGREES_PER_COUNT);
	
	if(*angle < 0)
		return CHANNEL_VALUE_MIDDLE + (-counts);
	else
		return CHANNEL_VALUE_MIDDLE - counts;
}

/*
 *	returns a count value minus the value CHANNEL_VALUE_COUNT_MIN
 */
int get_productive_counts(unsigned short* counts)
{
	return *counts - CHANNEL_VALUE_COUNT_MIN;
}

/*
 *	checks if the values in sum_signal_array are valid,
 *	if not the relevant value will replaced with the last valid of this one
 */
void check_sum_signal_values(void)
{
	int index = 0;
	
	//print(EXT_DEBUG_HANDLE, "start check_sum_signal_values\r\n");
	
	for(index = 0; index < MAX_CHANNELS; index++)
	{
		if(sum_signal_array[index] < CHANNEL_VALUE_COUNT_MIN)
			sum_signal_array[index] = last_valid_values[index];
	}
	
	last_valid_values[0] = sum_signal_array[0];
	last_valid_values[1] = sum_signal_array[1];
	last_valid_values[2] = sum_signal_array[2];
	last_valid_values[3] = sum_signal_array[3];
	last_valid_values[4] = sum_signal_array[4];
	last_valid_values[5] = sum_signal_array[5];
	
	if((sum_signal_array[0] <= CHANNEL_VALUE_COUNT_MIN + 200) && (sum_signal_array[3] <= CHANNEL_VALUE_COUNT_MIN + 200))
	{
		if(state == security_state)
		{
			state = operating_state;
			//print(EXT_DEBUG_HANDLE, "operating_state\r\n");
			#ifdef BLUETOOTH_ON
			if(bluetooth_receiver_active) deact_bluetooth_receive_int();
			#endif
		}
		else
		{
			state = security_state;
			//print(EXT_DEBUG_HANDLE, "security_state\r\n");
			#ifdef BLUETOOTH_ON
			if(!bluetooth_receiver_active) act_bluetooth_receive_int();
			#endif
		}
	}
	
	//print(EXT_DEBUG_HANDLE, "end check_sum_signal_values\r\n");
}

/*
 *	reads the sum_signal part for the filter switch and the angle controller switch 
 *	and set kalman_activated and angle_controller_activated to true or false
 */
void evaluate_rc_switch_values(void)
{
	//print(EXT_DEBUG_HANDLE, "start evaluate_rc_switch_values\r\n");
	
	if(sum_signal_array[CONTROLLER_SWITCH_INDEX] <= CONTROLLER_SWITCH_MIN)
		angle_controller_activated = FALSE;
	else if(sum_signal_array[CONTROLLER_SWITCH_INDEX] >= CONTROLLER_SWITCH_MAX)
		angle_controller_activated = TRUE;
	
	if(sum_signal_array[FILTER_SWITCH_INDEX] <= FILTER_SWITCH_MIN)
		kalman_activated = FALSE;
	else if(sum_signal_array[FILTER_SWITCH_INDEX] >= FILTER_SWITCH_MAX)
		kalman_activated = TRUE;
	
	//print(EXT_DEBUG_HANDLE, "end evaluate_rc_switch_values\r\n");
}

/*
 *	waits for a special stick combination(left stick down/right)
 */
void wait_for_sec_stick_comb(void)
{
	for(;;)
	{
		for(int i = 0; i < 1000000; i++);
		
		tooglesLED(RED_OB_LED);
		check_sum_signal_values();
		
		if((sum_signal_array[0] <= CHANNEL_VALUE_COUNT_MIN + 200) && (sum_signal_array[3] <= CHANNEL_VALUE_COUNT_MIN + 200))
			break;
	}
}



