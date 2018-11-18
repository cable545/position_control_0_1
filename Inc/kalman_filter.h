#ifndef _KALMAN_FILTER_
#define _KALMAN_FILTER_

// Q_t Rauschkovarianzmatrix
#define Q_ANGLE 0.01;					//0.001; Kombi ist weniger anfällig auf Vibrat
#define Q_GYRO  0.003;				//0.003; Kombi ist weniger anfällig auf Vibrat

#define Q_ANGLE_YAW 0.01
#define Q_GYRO_YAW 0.003

// R_t Rauschen des Sensors(Gyro)
#define R_ANGLE 0.03; //0.01;

typedef struct
{
	float x_angle;
	float x_bias;
	float p00, p01, p10, p11;
	float q0, q1;
	float r0;
}Kalman_Data;

void kalman_init(Kalman_Data *data, int is_yaw);
void kalman_calculate(Kalman_Data* data, float* angle, float* gyro_rate);
void kalman_init_all_angles(void);

#endif
