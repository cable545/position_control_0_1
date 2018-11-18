#include "main.h"

static char uart_data_array[INIT_DATA_BUFFER_LENGTH] = {0};
static char data_array[55];
static int uart_data_array_index = 0;

static int new_data = FALSE;
static int bluetooth_connected = FALSE;

extern float kp_angle_roll, ki_angle_roll, kp_rate_roll, ki_rate_roll;
extern float kp_angle_pitch, ki_angle_pitch, kp_rate_pitch, ki_rate_pitch;
extern float kp_angle_yaw, ki_angle_yaw, kp_rate_yaw, ki_rate_yaw;

extern int roll, pitch, yaw;
extern Controller_Values set_values;
extern float DELTA_T;

extern main_loop_states state;
extern Imu_Data acc_values, gyro_values, mag_values;

extern float x_gyro_offset, y_gyro_offset, z_gyro_offset;
extern float x_acc_offset, y_acc_offset, z_acc_offset;

int bluetooth_receiver_active = FALSE;

void UART1_IRQHandler(void)
{
	if(UART_S1_REG(UART1_BASE_PTR) & UART_S1_RDRF_MASK)
	{
		uart_data_array[uart_data_array_index] = UART_D_REG(UART1_BASE_PTR);
		
		if(++uart_data_array_index == INIT_DATA_BUFFER_LENGTH)
		{
			uart_data_array_index = 0;
			new_data = TRUE;
		}
	}
}

void compute_bluetooth_data(void)
{
	if(new_data)
	{
		switch(uart_data_array[0])
		{
			case CONNECT_MESSAGE_ID:
				send_initial_controller_data();
				break;
			
			case DEBUG_DATA_START_SIGNAL_MESSAGE_ID:
				bluetooth_connected = TRUE;
				printf("Bluetooth connected\r\n");
				break;
			
			case DISCONNECT_MESSAGE_ID:
				bluetooth_connected = FALSE;
				printf("Bluetooth disconnected\r\n");
				break;
			
			case DATA_ROLL_MESSAGE_ID:
				cp_buffer_to_var(&uart_data_array[1], &kp_angle_roll, &ki_angle_roll, &kp_rate_roll, &ki_rate_roll);
				break;
			
			case DATA_PITCH_MESSAGE_ID:
				cp_buffer_to_var(&uart_data_array[17], &kp_angle_pitch, &ki_angle_pitch, &kp_rate_pitch, &ki_rate_pitch);
				break;
			
			case DATA_YAW_MESSAGE_ID:
				cp_buffer_to_var(&uart_data_array[33], &kp_angle_yaw, &ki_angle_yaw, &kp_rate_yaw, &ki_rate_yaw);
				break;
			
			case DATA_ROLL_PITCH_MESSAGE_ID:
				cp_buffer_to_var(&uart_data_array[1], &kp_angle_roll, &ki_angle_roll, &kp_rate_roll, &ki_rate_roll);
				cp_buffer_to_var(&uart_data_array[17], &kp_angle_pitch, &ki_angle_pitch, &kp_rate_pitch, &ki_rate_pitch);
				break;
			
			case DATA_ROLL_YAW_MESSAGE_ID:
				cp_buffer_to_var(&uart_data_array[1], &kp_angle_roll, &ki_angle_roll, &kp_rate_roll, &ki_rate_roll);
				cp_buffer_to_var(&uart_data_array[33], &kp_angle_yaw, &ki_angle_yaw, &kp_rate_yaw, &ki_rate_yaw);
				break;
			
			case DATA_YAW_PITCH_MESSAGE_ID:
				cp_buffer_to_var(&uart_data_array[17], &kp_angle_pitch, &ki_angle_pitch, &kp_rate_pitch, &ki_rate_pitch);
				cp_buffer_to_var(&uart_data_array[33], &kp_angle_yaw, &ki_angle_yaw, &kp_rate_yaw, &ki_rate_yaw);
				break;
			
			case DATA_ALL_MESSAGE_ID:
				cp_buffer_to_var(&uart_data_array[1], &kp_angle_roll, &ki_angle_roll, &kp_rate_roll, &ki_rate_roll);
				cp_buffer_to_var(&uart_data_array[17], &kp_angle_pitch, &ki_angle_pitch, &kp_rate_pitch, &ki_rate_pitch);
				cp_buffer_to_var(&uart_data_array[33], &kp_angle_yaw, &ki_angle_yaw, &kp_rate_yaw, &ki_rate_yaw);
				break;
			case START_CALIB_MESSAGE_ID:
				start_calibration();
				break;
			
			default:
				break;										
		}
		
		new_data = FALSE;
	}
}

void send_initial_controller_data(void)
{
	data_array[0] = CONNECT_MESSAGE_RESPONSE_ID;
	
	cp_var_to_buffer(&data_array[1], &kp_angle_roll, &ki_angle_roll, &kp_rate_roll, &ki_rate_roll);
	cp_var_to_buffer(&data_array[17], &kp_angle_pitch, &ki_angle_pitch, &kp_rate_pitch, &ki_rate_pitch);
	cp_var_to_buffer(&data_array[33], &kp_angle_yaw, &ki_angle_yaw, &kp_rate_yaw, &ki_rate_yaw);
	
	send_data(data_array, INIT_DATA_BUFFER_LENGTH);
	init_bluetooth_data_array();
}

void cp_buffer_to_var(char* buffer, float* kp_angle, float* ki_angle, float* kp_rate, float* ki_rate)
{
	memcpy(kp_angle, &buffer[0], 4);
	memcpy(ki_angle, &buffer[4], 4);
	memcpy(kp_rate, &buffer[8], 4);
	memcpy(ki_rate, &buffer[12], 4);
}

void cp_var_to_buffer(char* buffer, float* kp_angle, float* ki_angle, float* kp_rate, float* ki_rate)
{
	memcpy(&buffer[0], kp_angle, 4);
	memcpy(&buffer[4], ki_angle, 4);
	memcpy(&buffer[8], kp_rate, 4);
	memcpy(&buffer[12], ki_rate, 4);
}

void act_bluetooth_receive_int(void)
{
	bluetooth_receiver_active = TRUE;
	char tmp;
	
	if(UART_S1_REG(UART1_BASE_PTR) & UART_S1_RDRF_MASK)
	{
		tmp = UART_D_REG(UART1_BASE_PTR);
		uart_data_array_index = 0;
	}
	
	enable_irq(INT_UART1 -16);
	set_irq_priority((INT_UART1 -16), 0);
}

void deact_bluetooth_receive_int(void)
{
	bluetooth_receiver_active = FALSE;
	
	disable_irq(INT_UART1 -16);
}

void send_debug_data(int calib_value)
{
	if(bluetooth_connected)
	{
		if(state == security_state)
		{
			data_array[0] = 5;
			
			memcpy(&data_array[4], &gyro_values.x, 4);
			memcpy(&data_array[8], &gyro_values.y, 4);
			memcpy(&data_array[12], &gyro_values.z, 4);
			memcpy(&data_array[16], &acc_values.x, 4);
			memcpy(&data_array[20], &acc_values.y, 4);
			memcpy(&data_array[24], &acc_values.z, 4);
			memcpy(&data_array[28], &mag_values.x, 4);
			memcpy(&data_array[32], &mag_values.y, 4);
			memcpy(&data_array[36], &mag_values.z, 4);
			memcpy(&data_array[40], &calib_value, 4);
			
			send_data(data_array, SENSOR_DATA_BUFFER_LENGTH);
		}
		else if(state == operating_state)
		{
			data_array[0] = 1;
			
			memcpy(&data_array[4], &roll, 4);
			memcpy(&data_array[8], &pitch, 4);
			memcpy(&data_array[12], &yaw, 4);
			memcpy(&data_array[16], &set_values.roll, 4);
			memcpy(&data_array[20], &set_values.pitch, 4);
			memcpy(&data_array[24], &set_values.yaw, 4);
			memcpy(&data_array[28], &DELTA_T, 4);
			
			send_data(data_array, DEBUG_DATA_BUFFER_LENGTH);
		}
	}
}

void send_data(char* data, int data_length)
{
	int index;
	
	for(index = 0; index < data_length; index++)
	{
		//print(EXT_DEBUG_HANDLE, "%d,", data[index]);
		uart_putchar(UART1_BASE_PTR, data[index]);
	}
	//print(EXT_DEBUG_HANDLE, "\r\n");
}

void send_offset_value_data(void)
{
	if(bluetooth_connected)
	{
		if(state == security_state)
		{
			data_array[0] = 3;
			
			memcpy(&data_array[4], &x_gyro_offset, 4);
			memcpy(&data_array[8], &y_gyro_offset, 4);
			memcpy(&data_array[12], &z_gyro_offset, 4);
			memcpy(&data_array[16], &x_acc_offset, 4);
			memcpy(&data_array[20], &y_acc_offset, 4);
			memcpy(&data_array[24], &z_acc_offset, 4);
	
			send_data(data_array, OFFSET_DATA_BUFFER_LENGTH);
		}
	}
}

void init_bluetooth_data_array()
{
	data_array[0] = 1;
	data_array[1] = 2;
	data_array[2] = 3;
	data_array[3] = 4;
}


