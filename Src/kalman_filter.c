#include "main.h"

extern float DELTA_T;

Kalman_Data roll_data, pitch_data , yaw_data;

void kalman_init(Kalman_Data *data, int is_yaw)
{
	data->x_angle = 0.0f;
	data->x_bias = 0.0f;
	
	// Init P to diagonal matrix with large values since
	// the initial state is not known
	data->p00 = 1000.0f;
	data->p01 = 0.0f;
	data->p10 = 0.0f;
	data->p11 = 1000.0f;
	
	if(is_yaw)
	{
		data->q0 = Q_ANGLE_YAW;
		data->q1 = Q_GYRO_YAW;
	}
	else
	{
		data->q0 = Q_ANGLE;
		data->q1 = Q_GYRO;
	}
		
	data->r0 = R_ANGLE;
}

void kalman_calculate(Kalman_Data* data, float* angle, float* gyro_rate)
{
	float y, S;
	float K_0, K_1;
	
	// predict step
  data->x_angle += DELTA_T * (*gyro_rate - data->x_bias);
	//data->x_bias = data->x_bias
	
	data->p00 -= DELTA_T * (data->p01 + data->p10 - DELTA_T * data->p11 - data->q0);
  data->p01 -= DELTA_T * data->p11;
	data->p10 -= DELTA_T * data->p11;
	data->p11 += DELTA_T * data->q1;
	
	// update step
  y = *angle - data->x_angle;		
	
  S = data->p00 + data->r0;				
  K_0 = data->p00 / S;
  K_1 = data->p10 / S;
  
  data->x_angle +=  K_0 * y;
  data->x_bias  +=  K_1 * y;
	
  data->p00 -= K_0 * data->p00;
  data->p01 -= K_0 * data->p01;
  data->p10 -= K_1 * data->p00;
  data->p11 -= K_1 * data->p01;
}

void kalman_init_all_angles(void)
{
	kalman_init(&roll_data, FALSE);
	kalman_init(&pitch_data, FALSE);
	kalman_init(&yaw_data, TRUE);
}
