#ifndef _GYRO_H_
#define	_GYRO_H_

#include "common.h"

#define GYRO_I2C_ADDRESS (0x6B<<1)
#define OFFSET_LOOPS 500
#define GYRO_RESOLUTION_500 0.0175 //resolution from gyro with 500dps

uint8 gyro_read_reg(uint8 reg_addr);
void gyro_write_reg(uint8 reg_addr, uint8 data);
void gyro_init(void);
void gyro_offset_calc(float* x_offset, float* y_offset, float* z_offset);
uint16 gyro_read(float* x, float* y, float* z);
//uint16 gyro_read_comp(float* x, float* y, float* z);

#endif
