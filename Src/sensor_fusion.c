#include "main.h"

extern Kalman_Data roll_data, pitch_data , yaw_data;
extern Imu_Data acc_values, gyro_values, mag_values;
extern int kalman_activated;

extern float DELTA_T;

static float roll_comp = 0.0, pitch_comp = 0.0, yaw_comp = 0.0;
static float gyro_roll = 0.0, gyro_pitch = 0.0, gyro_yaw = 0.0;
static float acc_roll = 0.0, acc_pitch = 0.0;
static float tilt_comp_yaw = 0.0;

int roll = 0, pitch = 0, yaw = 0;

/*
 *	complementary filter function to merge gyro and accelerometer or gyro and mag values
 *	to get a practical value for roll, pitch or yaw
 */
void complementary_filter(float* rate, float* angle, float* calc_angle)
{
	float a = TAU / (TAU + DELTA_T);
	
	*calc_angle = a * (*calc_angle + ((*rate) * DELTA_T)) + ((1 - a) * (*angle));
}

/*
 *	wrapper function for the hole sensor fusion
 */
void sensor_fusion(void)
{
	//print(EXT_DEBUG_HANDLE, "7\r\n");
	
	#ifdef GYRO_ANGLES
	l3gd_gyro_calc_current_angles(&gyro_roll, &gyro_pitch, &gyro_yaw, &gyro_values);
	#endif
	
	lsm303_acc_calc_current_angles(&acc_roll, &acc_pitch);
	
	if(kalman_activated)
	{
		kalman_calculate(&roll_data, &acc_roll, &gyro_values.x);
		kalman_calculate(&pitch_data, &acc_pitch, &gyro_values.y);
		
//		get_tilt_comp_heading(&roll_data.x_angle, &pitch_data.x_angle, &tilt_comp_yaw);
//		kalman_calculate(&yaw_data, &tilt_comp_yaw, &gyro_values.z);
		
		roll = (int)roll_data.x_angle;
		pitch = (int)pitch_data.x_angle;
		yaw = (int)yaw_data.x_angle;
	}
	else
	{
		complementary_filter(&gyro_values.x, &acc_roll, &roll_comp);
		complementary_filter(&gyro_values.y, &acc_pitch, &pitch_comp);
		
//		get_tilt_comp_heading(&roll_data.x_angle, &pitch_data.x_angle, &tilt_comp_yaw);
//		complementary_filter(&gyro_values.z, &tilt_comp_yaw, &yaw_comp);
		
		roll = (int)roll_comp;
		pitch = (int)pitch_comp;
		yaw = (int)yaw_comp;
	}
	
	//print(EXT_DEBUG_HANDLE, "8\r\n");
}


