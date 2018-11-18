#include "main.h"

static Imu_Data mag_values, acc_values, gyro_values;

/*
 *	a low pass filter to smoothing the measured data from the magnetometer
 */
void mag_low_pass_filter(Imu_Data* new_mag_values)
{
	mag_values.x = (new_mag_values->x * MAG_FILTERING_FACTOR) + (mag_values.x * (1.0 - MAG_FILTERING_FACTOR));
	mag_values.y = (new_mag_values->y * MAG_FILTERING_FACTOR) + (mag_values.y * (1.0 - MAG_FILTERING_FACTOR));
	mag_values.z = (new_mag_values->z * MAG_FILTERING_FACTOR) + (mag_values.z * (1.0 - MAG_FILTERING_FACTOR));
	
	new_mag_values->x = mag_values.x;
	new_mag_values->y = mag_values.y;
  new_mag_values->z = mag_values.z;
}

/*
 *	a low pass filter to smoothing the measured data from the accelerometer
 */
void acc_low_pass_filter(Imu_Data* new_acc_values)
{
	acc_values.x = (new_acc_values->x * ACC_FILTERING_FACTOR) + (acc_values.x * (1.0 - ACC_FILTERING_FACTOR));
	acc_values.y = (new_acc_values->y * ACC_FILTERING_FACTOR) + (acc_values.y * (1.0 - ACC_FILTERING_FACTOR));
	acc_values.z = (new_acc_values->z * ACC_FILTERING_FACTOR) + (acc_values.z * (1.0 - ACC_FILTERING_FACTOR));
	
	new_acc_values->x = acc_values.x;
	new_acc_values->y = acc_values.y;
  new_acc_values->z = acc_values.z;
}

/*
 *	a low pass filter to smoothing the measured data from the gyroscope
 */
void gyro_low_pass_filter(Imu_Data* new_gyro_values)
{
	gyro_values.x = (new_gyro_values->x * GYRO_FILTERING_FACTOR) + (gyro_values.x * (1.0 - GYRO_FILTERING_FACTOR));
	gyro_values.y = (new_gyro_values->y * GYRO_FILTERING_FACTOR) + (gyro_values.y * (1.0 - GYRO_FILTERING_FACTOR));
	gyro_values.z = (new_gyro_values->z * GYRO_FILTERING_FACTOR) + (gyro_values.z * (1.0 - GYRO_FILTERING_FACTOR));
	
	new_gyro_values->x = gyro_values.x;
	new_gyro_values->y = gyro_values.y;
  new_gyro_values->z = gyro_values.z;
}
