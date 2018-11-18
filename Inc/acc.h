#ifndef _ACC_H_
#define	_ACC_H_

#include "common.h"

#define ACC_I2C_ADDRESS (0x1d<<1)
#define SENSITIVITY_2G 4096 					//4096 counts/g
#define SENSITIVITY_4G 2048 					//2048 counts/g
#define SENSITIVITY_8G 1024 					//1024 counts/g

#define ACC_2G 0x00
#define ACC_4G 0x01
#define ACC_8G 0x02

#define ALPHA 0.5

uint8 acc_read_reg(uint8 reg_addr);
void acc_write_reg(uint8 reg_addr, uint8 data);

void accel_init(void);
uint16 accel_read(float* x, float* y, float* z);

uint16 accelReadX(float* xp);
uint16 accelReadY(float* yp);
uint16 accelReadZ(float* zp);

uint16 accel_rp_angle_filter(float* x_act, float* y_act, float* z_act, float* roll, float* pitch);
uint16 accel_rp_angle_filter(float* x_act, float* y_act, float* z_act, float* roll, float* pitch);

#endif
