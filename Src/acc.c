#include <arm_math.h>
#include "acc.h"
#include "common.h"
#include "i2c.h"

static void pause(void)
{
  for(int n = 0; n < 40; n++);
		__asm("nop");	
}

// lowlevel functions for the onboard accelerometer

uint8 acc_read_reg(uint8 reg_addr)
{
	uint8 result;

  i2c_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, ACC_I2C_ADDRESS | I2C_WRITE);
    
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_repeated_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, ACC_I2C_ADDRESS | I2C_READ);
  i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_set_rx_mode(I2C0_BASE_PTR);

	i2c_give_nack(I2C0_BASE_PTR);
	result = i2c_read_byte(I2C0_BASE_PTR);
	i2c_wait(I2C0_BASE_PTR);

	i2c_stop(I2C0_BASE_PTR);
  result = i2c_read_byte(I2C0_BASE_PTR);
	pause();
	return result;
}

void acc_write_reg(uint8 reg_addr, uint8 data)
{
	i2c_start(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, ACC_I2C_ADDRESS|I2C_WRITE);
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, data);
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_stop(I2C0_BASE_PTR);
	pause();
}

// hight level accelerometer functions 

void accel_init(void)
{
  i2c_init_acc();      //Initialize I2C modules
	
	// CTRL_REG1 0x2a enable standby mode
	acc_write_reg(0x2a, 0x00);
	
	// XYZ_DATA_CFG Register 0x0e select full scale range
	acc_write_reg(0x0E, ACC_8G);
	
	// CTRL_REG1 0x2a enable active mode
	acc_write_reg(0x2a, 0x01);
}

uint16 accel_read(float* x, float* y, float* z)
{
	signed short accel_x, accel_y, accel_z;
	
	if((acc_read_reg(0x00)&0xf) != 0)
	{
		accel_x   = acc_read_reg(0x01)<<8;
		accel_x  |= acc_read_reg(0x02);
		accel_x >>= 2;

		accel_y   = acc_read_reg(0x03)<<8;
		accel_y  |= acc_read_reg(0x04);
		accel_y >>= 2;

		accel_z   = acc_read_reg(0x05)<<8;
		accel_z  |= acc_read_reg(0x06);
		accel_z >>= 2;
		
		*x = (float)accel_x / SENSITIVITY_8G;
		*y = (float)accel_y / SENSITIVITY_8G;
		*z = (float)accel_z / SENSITIVITY_8G;
		
		return TRUE;
	}
	return FALSE;
}

uint16 accelReadX(float* xp)
{
	signed short accel_x;
	float x = 0.0;
	
	if((acc_read_reg(0x00)&0xf) != 0)
	{
		accel_x   = acc_read_reg(0x01)<<8;
    accel_x  |= acc_read_reg(0x02);
		accel_x >>= 2;
		
		x = accel_x;
		x /= SENSITIVITY_8G;
		*xp = x;
		
		return TRUE;
	}
	return FALSE;
}

uint16 accelReadY(float* yp) 
{
	signed short accel_y;
	float y = 0.0;
	
	if((acc_read_reg(0x00)&0xf) != 0)
	{
		accel_y   = acc_read_reg(0x03)<<8;
		accel_y  |= acc_read_reg(0x04);
		accel_y >>= 2;
		
		y = accel_y;
		y /= SENSITIVITY_8G;
		*yp = y;
		
		return TRUE;
	}
	return FALSE;
}

uint16 accelReadZ(float* zp)
{
	signed short accel_z;
	float z = 0.0;
	
	if((acc_read_reg(0x00)&0xf) != 0)
	{
		accel_z   = acc_read_reg(0x05)<<8;
		accel_z  |= acc_read_reg(0x06);
		accel_z >>= 2;
		
		z = accel_z;
		z /= SENSITIVITY_8G;
		*zp = z;
		
		return TRUE;
	}
	return FALSE;
}

uint16 accel_rp_angle_filter(float* x_act, float* y_act, float* z_act, float* roll, float* pitch)
{
	float xyz_values[3] = {0.0, 0.0, 0.0};
	uint16 return_value = FALSE;
	
	return_value = accel_read(x_act, y_act, z_act);
	
	if(return_value)
	{
    //Low Pass Filter
    *x_act = xyz_values[0] * ALPHA + (*x_act * (1.0 - ALPHA));
    *y_act = xyz_values[1] * ALPHA + (*y_act * (1.0 - ALPHA));
    *z_act = xyz_values[2] * ALPHA + (*z_act * (1.0 - ALPHA));
 
    //Roll & Pitch Equations
    *roll  = (atan2(*x_act, *z_act) * 180.0) / PI;
    *pitch = (atan2(-(*y_act), sqrt((*x_act) * (*x_act) + (*z_act) * (*z_act)))*180.0)/PI;
		
		return TRUE;
	}
	return FALSE;
}

