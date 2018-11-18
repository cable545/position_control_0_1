#ifndef EXTENDED_MATH
#define EXTENDED_MATH

#define DEG_RAD 0.017453292
#define RAD_DEG 57.29577951
#define MyFastCos(angle) MyFastSin((angle)+(90))

typedef struct
{
	float a;
	float b;
	float c;
	float d;
	
} QUATERNION;

uint8 abs_q(QUATERNION* q, float* q_abs);
uint8 norm_q(QUATERNION* q, QUATERNION* q_norm);
uint8 quat_to_eul(float* roll, float* pitch, float* yaw, QUATERNION* q);
void eul_to_quat(float* roll, float* pitch, float* yaw, QUATERNION* q);

float deg(float* rad);
float rad(float* deg);

float MyFastSin(float angle);

#endif
