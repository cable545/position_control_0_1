#include "main.h"

extern float DELTA_T;

extern unsigned short sum_signal_array[MAX_CHANNELS];
extern int roll, pitch, yaw;
extern Imu_Data gyro_values;

extern int angle_controller_activated;

float kp_angle_roll = 3.75;
float ki_angle_roll = 0.01;
float kp_rate_roll = 1.25;
float ki_rate_roll = 1.75;

float kp_angle_pitch = 3.75;//4.85;
float ki_angle_pitch = 0.01;//3.0;
float kp_rate_pitch = 1.25;//1.26;
float ki_rate_pitch = 1.75;//0.0;

float kp_angle_yaw = 0.0;
float ki_angle_yaw = 0.0;
float kp_rate_yaw = 1.75;
float ki_rate_yaw = 0.8;

Controller_Values set_values;

static float err_angle_sum_roll, err_rate_sum_roll, err_angle_sum_pitch, err_rate_sum_pitch, err_angle_sum_yaw, err_rate_sum_yaw;
static int yaw_set_value_angle_defined = FALSE;
static int yaw_set_value_angle;

/*
 *	starts a controller loop for roll, pitch and yaw
 */
void controller(void)
{
	int actuating_var_roll, actuating_var_pitch, actuating_var_yaw;
	int throttle_mot_a, throttle_mot_b, throttle_mot_c, throttle_mot_d;
	
	sum_signal_array_to_structure(&set_values);
	
	if(set_values.throttle < CHANNEL_VALUE_COUNT_MIN + 200)
	{
//		if(!yaw_set_value_angle_defined)
//		{
//			yaw_set_value_angle = yaw;
//			yaw_set_value_angle_defined = TRUE;
//		}
		
		set_motor_throttle(CHANNEL_VALUE_COUNT_MIN, MOTOR_A);
		set_motor_throttle(CHANNEL_VALUE_COUNT_MIN, MOTOR_B);
		set_motor_throttle(CHANNEL_VALUE_COUNT_MIN, MOTOR_C);
		set_motor_throttle(CHANNEL_VALUE_COUNT_MIN, MOTOR_D);
	}
	else
	{
		actuating_var_roll = pid_roll(set_values.roll, roll, gyro_values.x);
		actuating_var_pitch = pid_pitch(set_values.pitch, pitch, gyro_values.y);
		
//		if(set_values.yaw < (CHANNEL_VALUE_MIDDLE - 5) || set_values.yaw > (CHANNEL_VALUE_MIDDLE + 5))
//		{
		actuating_var_yaw = pid_yaw_rate(set_values.yaw, gyro_values.z);
//			yaw_set_value_angle_defined = FALSE;
//		}
//		else
//		{
//			
//			if(!yaw_set_value_angle_defined)
//			{
//				
//				yaw_set_value_angle = yaw;
//				yaw_set_value_angle_defined = TRUE;
//			}
//			
//			actuating_var_yaw = -pid_yaw(yaw_set_value_angle, yaw, gyro_values.z);
//		}
		
		throttle_mot_a = set_values.throttle + actuating_var_pitch - actuating_var_yaw;
		throttle_mot_b = set_values.throttle - actuating_var_roll + actuating_var_yaw;
		throttle_mot_c = set_values.throttle - actuating_var_pitch - actuating_var_yaw;
		throttle_mot_d = set_values.throttle + actuating_var_roll + actuating_var_yaw;
		
		set_motor_throttle(throttle_mot_a, MOTOR_A);
		set_motor_throttle(throttle_mot_b, MOTOR_B);
		set_motor_throttle(throttle_mot_c, MOTOR_C);
		set_motor_throttle(throttle_mot_d, MOTOR_D);
	}
}

/*
 *	the core pid angle/rate controller implementation for roll
 */ 
float pid_roll(int set_value, int act_value_angle, float gyro_value)
{
	int error_angle;
	float set_value_rate, error_rate, output;
	
	if(angle_controller_activated)
	{
		set_value = -map(set_value, ROLL_R_MIN, ROLL_R_MAX, ROLL_A_MIN, ROLL_A_MAX);
		
		error_angle = set_value - act_value_angle;
		err_angle_sum_roll += (error_angle * DELTA_T);
	
		if(err_angle_sum_roll > MAX_ACTUATING_VALUE) err_angle_sum_roll = MAX_ACTUATING_VALUE;
		else if(err_angle_sum_roll < MIN_ACTUATING_VALUE) err_angle_sum_roll = MIN_ACTUATING_VALUE;
	
		set_value_rate = kp_angle_roll * error_angle + ki_angle_roll * err_angle_sum_roll;
		set_value_rate = map(set_value_rate, MIN_ACTUATING_VALUE, MAX_ACTUATING_VALUE, ATTITUDE_MODE_RATE_MIN, ATTITUDE_MODE_RATE_MAX);
	
		error_rate = set_value_rate - gyro_value;
		err_rate_sum_roll += (error_rate * DELTA_T);
		
		if(err_rate_sum_roll > MAX_ACTUATING_VALUE) err_rate_sum_roll = MAX_ACTUATING_VALUE;
		else if(err_rate_sum_roll < MIN_ACTUATING_VALUE) err_rate_sum_roll = MIN_ACTUATING_VALUE;
	
		output = kp_rate_roll * error_rate + ki_rate_roll * err_rate_sum_roll;
	}
	else
	{
		set_value = -map(set_value, ROLL_R_MIN, ROLL_R_MAX, RATE_MODE_RATE_MIN, RATE_MODE_RATE_MAX);
	
		error_rate = set_value - gyro_value;
		err_rate_sum_roll += (error_rate * DELTA_T);
		
		if(err_rate_sum_roll > MAX_ACTUATING_VALUE) err_rate_sum_roll = MAX_ACTUATING_VALUE;
		else if(err_rate_sum_roll < MIN_ACTUATING_VALUE) err_rate_sum_roll = MIN_ACTUATING_VALUE;
	
		output = kp_rate_roll * error_rate + ki_rate_roll * err_rate_sum_roll;
	}
	
	if(output > MAX_ACTUATING_VALUE) output = MAX_ACTUATING_VALUE;
	else if(output < MIN_ACTUATING_VALUE) output = MIN_ACTUATING_VALUE;

	return output;
}

/*
 *	the core pid angle/rate controller implementation for pitch
 */ 
float pid_pitch(int set_value, int act_value_angle, float gyro_value)
{
	int error_angle;
	float set_value_rate, error_rate, output;
	
	if(angle_controller_activated)
	{
		set_value = -map(set_value, PITCH_R_MIN, PITCH_R_MAX, PITCH_A_MIN, PITCH_A_MAX);
		
		error_angle = set_value - act_value_angle;
		err_angle_sum_pitch += (error_angle * DELTA_T);
	
		if(err_angle_sum_pitch > MAX_ACTUATING_VALUE) err_angle_sum_pitch = MAX_ACTUATING_VALUE;
		else if(err_angle_sum_pitch < MIN_ACTUATING_VALUE) err_angle_sum_pitch = MIN_ACTUATING_VALUE;
	
		set_value_rate = kp_angle_pitch * error_angle + ki_angle_pitch * err_angle_sum_pitch;
		set_value_rate = map(set_value_rate, MIN_ACTUATING_VALUE, MAX_ACTUATING_VALUE, ATTITUDE_MODE_RATE_MIN, ATTITUDE_MODE_RATE_MAX);
	
		error_rate = set_value_rate - gyro_value;
		err_rate_sum_pitch += (error_rate * DELTA_T);
		
		if(err_rate_sum_pitch > MAX_ACTUATING_VALUE) err_rate_sum_pitch = MAX_ACTUATING_VALUE;
		else if(err_rate_sum_pitch < MIN_ACTUATING_VALUE) err_rate_sum_pitch = MIN_ACTUATING_VALUE;
	
		output = kp_rate_pitch * error_rate + ki_rate_roll * err_rate_sum_pitch;
	}
	else
	{
		set_value = -map(set_value, PITCH_R_MIN, PITCH_R_MAX, RATE_MODE_RATE_MIN, RATE_MODE_RATE_MAX);
	
		error_rate = set_value - gyro_value;
		err_rate_sum_pitch += (error_rate * DELTA_T);
		
		if(err_rate_sum_pitch > MAX_ACTUATING_VALUE) err_rate_sum_pitch = MAX_ACTUATING_VALUE;
		else if(err_rate_sum_pitch < MIN_ACTUATING_VALUE) err_rate_sum_pitch = MIN_ACTUATING_VALUE;
	
		output = kp_rate_pitch * error_rate + ki_rate_pitch * err_rate_sum_pitch;
	}
	
	if(output > MAX_ACTUATING_VALUE) output = MAX_ACTUATING_VALUE;
	else if(output < MIN_ACTUATING_VALUE) output = MIN_ACTUATING_VALUE;
	
	return output;
}

/*
 *	the core pid angle/rate controller implementation for yaw
 */ 
float pid_yaw(int set_value_angle, int act_value_angle, float gyro_value)
{
	int error_angle;
	float set_value_rate, error_rate, output;
	
	error_angle = set_value_angle - act_value_angle;
	err_angle_sum_pitch += (error_angle * DELTA_T);
	
	set_value_rate = kp_angle_yaw * error_angle + ki_angle_yaw * err_angle_sum_pitch;
	error_rate = set_value_rate - gyro_value;
	err_rate_sum_pitch += (error_rate * DELTA_T);
	
	output = kp_rate_yaw * error_rate + ki_rate_yaw * err_rate_sum_pitch;

	return output;
}

/*
 *	the core pid rate controller implementation only for yaw rate
 */ 
float pid_yaw_rate(int set_value_rate, float gyro_value)
{
	float error_rate, output;
	
	set_value_rate = -map(set_value_rate, YAW_R_MIN, YAW_R_MAX, YAW_RATE_MIN, YAW_RATE_MAX);
	
	error_rate = set_value_rate - gyro_value;
	err_rate_sum_yaw += (error_rate * DELTA_T);
	
	if(err_rate_sum_yaw > 200) err_rate_sum_yaw = 200;
	else if(err_rate_sum_yaw < -200) err_rate_sum_yaw = -200;
	
	output = kp_rate_yaw * error_rate + ki_rate_yaw * err_rate_sum_yaw;

	if(output > 200) output = 200;
	else if(output < -200) output = -200;
	
	return output;
}

/*
 *	copies the sum_signal_array to a more human readable structure
 */
void sum_signal_array_to_structure(Controller_Values* structure)
{
	structure->throttle = sum_signal_array[0];
	structure->roll = sum_signal_array[1];
	structure->pitch = sum_signal_array[2];
	structure->yaw = sum_signal_array[3];
}

/*
 *	a function to drop down the received throttle value
 */
unsigned short set_down_motor_perf(unsigned short throttle)
{
	if(throttle < CHANNEL_VALUE_COUNT_MIN)
	{
		return CHANNEL_VALUE_COUNT_MIN;
	}
	else
	{
		return ((throttle - 1643) / 2) + 1643;
	}
}

/*
 *	sets the controller params with the values from the receiver inputs
 */
//void set_controller_params(void)
//{
//	int i;
//	
//	i = (sum_signal_array[4] - 1652) / 40;
//	
//	kp_angle = 0.1 * i;
//	 
//	i = (sum_signal_array[5] - 1652) / 40;
//	
//	kp_rate = 0.1 * i;
//	
//	i = (sum_signal_array[6] - 1652) / 40;
//	
//	ki_angle = 3.0 + (0.1 * i);
//}

