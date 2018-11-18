#ifndef CONTROLLER_H
#define CONTROLLER_H

// a count of 25 correspond to 2 degrees
#define CONTROLLER_2DEG_RANGE 25

typedef struct
{
	int32 throttle;
	int32 roll;
	int32 pitch;
	int32 yaw;
} Controller_Values;

#define MAX_ACTUATING_VALUE 400
#define MIN_ACTUATING_VALUE -400

#define ATTITUDE_MODE_RATE_MAX 200
#define ATTITUDE_MODE_RATE_MIN -200

#define RATE_MODE_RATE_MAX 150
#define RATE_MODE_RATE_MIN -150

#define YAW_RATE_MIN -150
#define YAW_RATE_MAX 150

void controller(void);
float pid_roll(int set_value, int act_value_angle, float gyro_value);
float pid_pitch(int set_value, int act_value_angle, float gyro_value);
float pid_yaw(int set_value_angle, int act_value_angle, float gyro_value);
float pid_yaw_rate(int set_value_rate, float gyro_value);
void sum_signal_array_to_structure(Controller_Values* structure);
unsigned short set_down_motor_perf(unsigned short throttle);
void set_controller_params(void);

#endif
