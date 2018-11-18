#include "main.h"

/*-----------------------------------------------------------------------------------------------------*/
/*	                             functions for calculation with quaternions                            */
/*-----------------------------------------------------------------------------------------------------*/

/* 
 *  calculates the absolute value from a given quaternion
 */
uint8 abs_q(QUATERNION* q, float* q_abs)
{
	if(arm_sqrt_f32((q->a * q->a) + (q->b * q->b) + (q->c * q->c) + (q->d * q->d), q_abs) == ARM_MATH_SUCCESS)
		return TRUE;
	else
		return FALSE;
}

/* 
 *  calculates the normalized quaternion from a given quaternion
 */
uint8 norm_q(QUATERNION* q, QUATERNION* q_norm)
{
	float q_abs;
	
	if(!abs_q(q, &q_abs))
		return FALSE;
	
//	printf("q_abs: %lf\r\n", q_abs);
	
	q_norm->a = q->a / q_abs;
	q_norm->b = q->b / q_abs;
	q_norm->c = q->c / q_abs;
	q_norm->d = q->d / q_abs;
	
	return TRUE;
}

/* 
 *  calculates the euler values from a given quaternion
 */
uint8 quat_to_eul(float* roll, float* pitch, float* yaw, QUATERNION* q)
{
	QUATERNION q_norm;
	float a_2, b_2, c_2, d_2; 
	
	if(!norm_q(q, &q_norm))
		return FALSE;
	
	a_2 = q_norm.a * q_norm.a;
	b_2 = q_norm.b * q_norm.b;
	c_2 = q_norm.c * q_norm.c;
	d_2 = q_norm.d * q_norm.d;
	
	*roll  = atan2(2*(q_norm.c*q_norm.d + q_norm.a*q_norm.b), a_2 - b_2 - c_2 + d_2);
	*pitch = asin(-2*(q_norm.b*q_norm.d - q_norm.a*q_norm.c));
	*yaw   = atan2(2*(q_norm.b*q_norm.c + q_norm.a*q_norm.d), a_2 + b_2 - c_2 - d_2);
	
	return TRUE;
}

/* 
 *  calculates a quaternion from the given euler values in rad
 */
void eul_to_quat(float* roll, float* pitch, float* yaw, QUATERNION* q)
{
	float c_roll, c_pitch, c_yaw, s_roll, s_pitch, s_yaw, c_yaw_pitch, s_yaw_pitch;

	c_roll = cos(*roll / 2);
	c_pitch = cos(*pitch / 2);
	c_yaw = cos(*yaw / 2);
	
	s_roll = sin(*roll / 2);
	s_pitch = sin(*pitch / 2);
	s_yaw = sin(*yaw / 2);
	
	c_yaw_pitch = c_yaw * c_pitch;
	s_yaw_pitch = s_yaw * s_pitch;
	
	q->a = c_yaw_pitch * c_roll + s_yaw_pitch * s_roll;
	q->b = c_yaw_pitch * s_roll - s_yaw_pitch * c_roll;
	q->c = c_yaw * s_pitch * c_roll + s_yaw * c_pitch * s_roll;
	q->d = s_yaw * c_pitch * c_roll - c_yaw * s_pitch * s_roll;
}

/*-----------------------------------------------------------------------------------------------------*/
/*	                                            basic functions                                        */
/*-----------------------------------------------------------------------------------------------------*/

float deg(float* rad)
{
	return *rad * 180 / PI;
}

float rad(float* deg)
{
	return *deg * PI / 180;
}

/*-----------------------------------------------------------------------------------------------------*/
/*	                                       fast sin & cos functions                                    */
/*-----------------------------------------------------------------------------------------------------*/

float hollyConstant = 0.017453292519943295769236907684886;

float sinTable[] = {
	0.0,                                    //sin(0)
	0.17364817766693034885171662676931 ,    //sin(10)
	0.34202014332566873304409961468226 ,    //sin(20)
	0.5 ,                                    //sin(30)
	0.64278760968653932632264340990726 ,    //sin(40)
	0.76604444311897803520239265055542 ,    //sin(50)
	0.86602540378443864676372317075294 ,    //sin(60)
	0.93969262078590838405410927732473 ,    //sin(70)
	0.98480775301220805936674302458952 ,    //sin(80)
	1.0                                     //sin(90)
};

float cosTable[] = {
	1.0 ,                                    //cos(0)
	0.99984769515639123915701155881391 ,    //cos(1)
	0.99939082701909573000624344004393 ,    //cos(2)
	0.99862953475457387378449205843944 ,    //cos(3)
	0.99756405025982424761316268064426 ,    //cos(4)
	0.99619469809174553229501040247389 ,    //cos(5)
	0.99452189536827333692269194498057 ,    //cos(6)
	0.99254615164132203498006158933058 ,    //cos(7)
	0.99026806874157031508377486734485 ,    //cos(8)
	0.98768834059513772619004024769344         //cos(9)
};

float MyFastSin(float angle)
{
	int a = angle * 0.1f;
	float b = angle - 10 * a;

	return sinTable[a] * cosTable[(int)b] + b * hollyConstant * sinTable[9-a];
}
