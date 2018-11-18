#include <stdio.h>
#include <math.h>
#include "mag.h"
#include "i2c.h"

// Magnetometer variables
int mag_x, mag_y, mag_z;
int Mresultx, Mresulty, Mresultz;
int Mtestx,Mtesty,Mtestz;

float scale_x, scale_y, scale_z;

static void pause(void)
{
  for(int n = 0; n < 150; n++);
		__asm("nop");	
}

// lowlevel functions for the onboard magnetometer

uint8 mag_read_reg(uint8 addr)
{
  uint8 result;

  i2c_start(I2C0_BASE_PTR);
  i2c_write_byte(I2C0_BASE_PTR, MAG_I2C_ADDRESS | I2C_WRITE);
  
  i2c_wait(I2C0_BASE_PTR);
  i2c_get_ack(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, addr);
  i2c_wait(I2C0_BASE_PTR);
  i2c_get_ack(I2C0_BASE_PTR);

  i2c_repeated_start(I2C0_BASE_PTR);
  i2c_write_byte(I2C0_BASE_PTR, MAG_I2C_ADDRESS | I2C_READ);
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

void mag_write_reg(uint8 addr, uint8 data)
{
	i2c_start(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, MAG_I2C_ADDRESS|I2C_WRITE);
  i2c_wait(I2C0_BASE_PTR);
  i2c_get_ack(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, addr);
  i2c_wait(I2C0_BASE_PTR);
  i2c_get_ack(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, data);
  i2c_wait(I2C0_BASE_PTR);
  i2c_get_ack(I2C0_BASE_PTR);

  i2c_stop(I2C0_BASE_PTR);
  pause();
}

// hight level magnetometer functions 

uint8 mag_init(void)
{
	i2c_init_mag();      //Initialize I2C modules
	mag_write_reg(MAG_CTRL_REG1,0x01); //Active, 80Hz output rate
	if(mag_read_reg(MAG_WHO_AM_I) != 0xc4) return FALSE;
	
	return TRUE;
}

uint8 mag_read(float* x, float* y, float* z)
{
	int16 x_t, y_t, z_t;
	
  if((mag_read_reg(MAG_DR_STATUS) & 0xf) == 0xf)
  {    
    x_t  = mag_read_reg(MAG_OUT_X_MSB)<<8;
    x_t |= mag_read_reg(MAG_OUT_X_LSB);

    y_t  = mag_read_reg(MAG_OUT_Y_MSB)<<8;
    y_t |= mag_read_reg(MAG_OUT_Y_LSB);

    z_t  = mag_read_reg(MAG_OUT_Z_MSB)<<8;
    z_t |= mag_read_reg(MAG_OUT_Z_LSB);
		
		*x = ((float)x_t - OM_X_OFFSET);
		*y = ((float)y_t - OM_Y_OFFSET);
		*z = ((float)z_t - OM_Z_OFFSET);
		
		return TRUE;
  }
	return FALSE;
}

uint16 mag_read_x(signed short* x)
{
	if((mag_read_reg(0x00)&0xf) != 0)
	{
		*x  = mag_read_reg(0x01)<<8;
		*x |= mag_read_reg(0x02);
		
		return TRUE;
	}
	return FALSE;
}

uint16 mag_read_y(signed short* y)
{
	if((mag_read_reg(0x00)&0xf) != 0)
	{
		*y  = mag_read_reg(0x03)<<8;
		*y |= mag_read_reg(0x04);
		
		return TRUE;
	}
	return FALSE;
}

uint16 mag_read_z(signed short* z)
{
	if((mag_read_reg(0x00)&0xf) != 0)
	{
		*z  = mag_read_reg(0x05)<<8;
		*z |= mag_read_reg(0x06);
		
		return TRUE;
	}
	return FALSE;
}

//void get_heading(void)
//{
//	float angle;
//	signed short x, y, z;
//	
////	const int x_offset = 0;
////  const int y_offset = 0;
//	
//	mag_read(&x, &y, &z);
//	
//	if((x == 0) && (y < 0))
//		angle = PI / 2.0;
//	
//	if((x == 0) && (y > 0))
//		angle = 3.0 * PI / 2.0;
//	
//	if(x < 0)
//		angle = PI - atan2((float)y,(float)x);
//	
//	if((x > 0) && (y < 0))
//		angle = - atan2((float)y,(float)x);
//	
//	if((x > 0) && (y > 0))
//		angle = 2.0 * PI - atan2((float)y,(float)x);
//	
////	angle = atan2((float)y,(float)x);
//	angle = angle * (180 / 3.14159265);
//	
////	if (angle < 0) angle = angle + 360;
////  if (angle >= 360) angle = angle - 360;
//	
//	printf("Heading Winkel: %lf\n", angle);
//}

void create_scale_factors(void)
{
	signed short x_min, x_max, x[CREATE_SCALE_FACT_SIZE];
	signed short y_min, y_max, y[CREATE_SCALE_FACT_SIZE];
	signed short z_min, z_max, z[CREATE_SCALE_FACT_SIZE];
	
	for(int i = 0; i < CREATE_SCALE_FACT_SIZE; i++)
	{
		mag_read_x(&x[i]);
		mag_read_y(&y[i]);
		mag_read_z(&z[i]);
		
		if(i == 0)
		{
			x_min = x[i];
			x_max = x[i];
		
			y_min = y[i];
			y_max = y[i];
			
			z_min = z[i];
			z_max = z[i];
		}
		
		
		if(x_max < x[i]) {x_max = x[i];}
		if(x_min > x[i]) {x_min = x[i];}
		
		if(y_max < y[i]) {y_max = y[i];}
		if(y_min > y[i]) {y_min = y[i];}
		
		if(z_max < z[i]) {z_max = z[i];}
		if(z_min > z[i]) {z_min = z[i];}
		
		printf("x_max: %d, x_min: %d\n", x_max, x_min);
		printf("y_max: %d, y_min: %d\n", y_max, y_min);
		printf("z_max: %d, z_min: %d\n", z_max, z_min);
	}
	
	scale_x = 1.0 / (x_max - x_min);
	scale_y = 1.0 / (y_max - y_min);
	scale_z = 1.0 / (z_max - z_min);
	
	printf("create scale factors ready\n");
	printf("x: %lf, y: %lf, z:%lf\n", scale_x, scale_y, scale_z);
}

void test(void)
{
	signed short x, y, z;
	int angle;
	
	mag_read_x(&x);
	mag_read_y(&y);
	mag_read_z(&z);
	angle = (atan2((double)y,(double)x))*180/3.1456;
	//angle= atan2((double)y,(double)x) * (180 / 3.14159265) + 180;
//	printf("\n Magenotometer Values");
//  printf("\nx = %d \t",x);
//  printf("y = %d \t",y);
//  printf("z = %d \n",z);
	printf("heading = %d \n",angle);
}

void get_calibrate_values_onboard_mag()
{
	int index, i;
	float min_x, min_y, min_z, max_x, max_y, max_z, mag_values[3];
	
	for(index = 0; index < MAG_OFFSET_LOOPS; index++)
	{
		for(i = 0; i < 500000; i++);
		printf(".");
		
		mag_read(&mag_values[0], &mag_values[1], &mag_values[2]);
		if(index == 0)
		{
			min_x = mag_values[0]; 
			max_x = mag_values[0];
			
			min_y = mag_values[1]; 
			max_y = mag_values[1];
			
			min_z = mag_values[2]; 
			max_z = mag_values[2];
		}
		else
		{
			if(mag_values[0] < min_x) min_x = mag_values[0];
			if(mag_values[0] > max_x) max_x = mag_values[0];
			
			if(mag_values[1] < min_y) min_y = mag_values[1];
			if(mag_values[1] > max_y) max_y = mag_values[1];
			
			if(mag_values[2] < min_z) min_z = mag_values[2];
			if(mag_values[2] > max_z) max_z = mag_values[2];
		}
	}
	printf("\r\n");
	printf("min_x: %lf   max_x: %lf   \r\n", min_x, max_x);
	printf("min_y: %lf   max_y: %lf   \r\n", min_y, max_y);
	printf("min_z: %lf   max_z: %lf   \r\n", min_z, max_z);
}
