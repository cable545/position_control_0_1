#ifndef _MAG_H_
#define _MAG_H_

#include "common.h"

#define MAG_I2C_ADDRESS (0x0E<<1)		//
#define CREATE_SCALE_FACT_SIZE 200

#define MAG_DR_STATUS 0x00
#define MAG_OUT_X_MSB 0x01
#define MAG_OUT_X_LSB 0x02
#define MAG_OUT_Y_MSB 0x03
#define MAG_OUT_Y_LSB 0x04
#define MAG_OUT_Z_MSB 0x05
#define MAG_OUT_Z_LSB 0x06
#define MAG_WHO_AM_I  0x07
#define MAG_SYSMOD    0x08
#define MAG_OFF_X_MSB 0x09
#define MAG_OFF_X_LSB 0x0A
#define MAG_OFF_Y_MSB 0x0B
#define MAG_OFF_Y_LSB 0x0C
#define MAG_OFF_Z_MSB 0x0D
#define MAG_OFF_Z_LSB 0x0E
#define MAG_DIE_TEMP  0x0F
#define MAG_CTRL_REG1 0x10
#define MAG_CTRL_REG2 0x11

#define MAG_OFFSET_LOOPS 1000
#define TO_MICRO_TESLA 0.1

#define OM_X_OFFSET (-833.0F)
#define OM_Y_OFFSET (1138.5F)
#define OM_Z_OFFSET (2973.0F)

#define OM_MIN_X -1295
#define OM_MAX_X -371
#define OM_MIN_Y 656
#define OM_MAX_Y 1621
#define OM_MIN_Z 2529
#define OM_MAX_Z 3417


uint8 mag_read_reg(uint8 addr);
void mag_write_reg(uint8 addr, uint8 data);

uint8 mag_init(void);
uint8 mag_read(float* x, float* y, float* z);
uint16 mag_read_x(signed short* x);
uint16 mag_read_y(signed short* y);
uint16 mag_read_z(signed short* z);
//void get_heading(void);
void create_scale_factors(void);
void test(void);
void get_calibrate_values_onboard_mag(void);

#endif
