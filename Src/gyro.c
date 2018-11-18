#include "gyro.h"
#include "i2c.h"
#include <stdio.h>

static void pause(void)
{
  for(int n = 0; n < 40; n++);
		__asm("nop");	
}

// lowlevel functions for the external gyroscope L3GD20H 

uint8 gyro_read_reg(uint8 reg_addr)
{
	uint8 result;

  i2c_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, GYRO_I2C_ADDRESS | I2C_WRITE);
    
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_repeated_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, GYRO_I2C_ADDRESS | I2C_READ);
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

void gyro_write_reg(uint8 reg_addr, uint8 data)
{
	i2c_start(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, GYRO_I2C_ADDRESS | I2C_WRITE);
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

// hight level gyro functions 

void gyro_init(void)
{
	printf("am Anfang gyro_init \n");	
	i2c_init_ex_gyro();      //Initialize I2C modules
	
	/* CTRL1 register 0x20 
	 * enable X and Y axis 
	 * normal mode
	 */
	printf("Mitte gyro_init \n");	
	gyro_write_reg(0x20, 0x0B);
	printf("nach Mitte gyro_init \n");	
	/* CTRL4 register 0x23
   * Full scale selection to 500 dps
   */	
	gyro_write_reg(0x23, 0x10);
	printf("am Ende gyro_init \n");	
}

uint16 gyro_read(float* x, float* y, float* z)
{
	signed short gyro_x, gyro_y, gyro_z;
	
	// read status register and check if data available
	if((gyro_read_reg(0x27)&0x8) != 0)
	{
		gyro_x   = gyro_read_reg(0x29)<<8;
		gyro_x  |= gyro_read_reg(0x28);

		gyro_y   = gyro_read_reg(0x2B)<<8;
		gyro_y  |= gyro_read_reg(0x2A);

		gyro_z   = gyro_read_reg(0x2D)<<8;
		gyro_z  |= gyro_read_reg(0x2C);
		
		*x = gyro_x;
		*y = gyro_y;
		*z = gyro_z;
		
		return TRUE;
	}
	
	return FALSE;
}

void gyro_offset_calc(float* x_offset, float* y_offset, float* z_offset)
{
	uint16 index;
	float x, y, z;
	
	// offset calculation
	for(index = 0; index < OFFSET_LOOPS; index++)
	{
		gyro_read(&x, &y, &z);
		*x_offset += x;
		*y_offset += y;
		*z_offset += z;
	}
	
	*x_offset /= OFFSET_LOOPS;
	*y_offset /= OFFSET_LOOPS;
	*z_offset /= OFFSET_LOOPS;
	
	printf("am Ende gyro offset \n");	
}

//uint16 gyro_read_comp(float* x, float* y, float* z)
//{
//	if(gyro_read(x, y, z))
//	{
//		*x -= x_offset;
//		*y -= y_offset;
//		*z -= z_offset;
//		
//		*x *= GYRO_RESOLUTION_500;
//		*y *= GYRO_RESOLUTION_500;
//		*z *= GYRO_RESOLUTION_500;
//		
//		return TRUE;
//	}
//	
//	return FALSE;
//}
