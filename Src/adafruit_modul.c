#include "main.h"

extern float DELTA_T;

// container for the sensor values
Imu_Data acc_values, gyro_values, mag_values;

static float _lsm303Accel_MG_LSB     = 0.001F;   // 1, 2, 4 or 12 mg per lsb

static float cal_matrix[3][3]={{ 1.044623, -0.011422, -0.004677},
                          {-0.011422,  1.055071, -0.012410},
                          {-0.004677, -0.012410,  1.154486}};
	
static float cal_offsets[3] = {-16.127077, 83.156128, 64.799910};

static float _lsm303Mag_Gauss_LSB_XY = 1100.0F;  // Varies with gain
static float _lsm303Mag_Gauss_LSB_Z  = 980.0F;   // Varies with gain
static lsm303MagGain _magGain;
static uint8 _autoRangeEnabled = FALSE;

float x_gyro_offset = -109.564072, y_gyro_offset = -43.924377, z_gyro_offset = -139.007294;
float x_acc_offset = -0.002118, y_acc_offset = 0.039259, z_acc_offset = 0.039753;

/*-----------------------------------------------------------------------------------------------------*/
/*	                             functions for the adafruit accelerometer                              */
/*-----------------------------------------------------------------------------------------------------*/

/* 
 *  reads 1 byte data from the given reg_addr and returns them
 */
uint8 lsm303_acc_read_reg(uint8 reg_addr, uint8* result)
{
	//float times[4];
	
  i2c_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, LSM303_ADDRESS_ACCEL | I2C_WRITE);
	//start_timer();
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[0] = stop_timer();
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	//start_timer();
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[1] = stop_timer();
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_repeated_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, LSM303_ADDRESS_ACCEL | I2C_READ);
	//start_timer();
  if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[2] = stop_timer();
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_set_rx_mode(I2C0_BASE_PTR);

	i2c_give_nack(I2C0_BASE_PTR);
	*result = i2c_read_byte(I2C0_BASE_PTR);
	//start_timer();
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[3] = stop_timer();
	i2c_stop(I2C0_BASE_PTR);
  *result = i2c_read_byte(I2C0_BASE_PTR);
	pause();
	
	//printf("A %lf %lf %lf %lf\r\n", times[0], times[1], times[2], times[3]);
	
	return TRUE;
}

/* 
 *  writes 1 byte data to the given reg_addr
 */
uint8 lsm303_acc_write_reg(uint8 reg_addr, uint8 data)
{
//	print(STDOUT_HANDLE, "marke 1.1\r\n");
//	i2c_start(I2C0_BASE_PTR);
//	print(STDOUT_HANDLE, "marke 1.2\r\n");
//  i2c_write_byte(I2C0_BASE_PTR, LSM303_ADDRESS_ACCEL | I2C_WRITE);
//	i2c_wait(I2C0_BASE_PTR);
//	i2c_get_ack(I2C0_BASE_PTR);
//	print(STDOUT_HANDLE, "marke 1.3\r\n");
//	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
//	print(STDOUT_HANDLE, "marke 1.4\r\n");
//	i2c_wait(I2C0_BASE_PTR);
//	print(STDOUT_HANDLE, "marke 1.5\r\n");
//	i2c_get_ack(I2C0_BASE_PTR);
//	print(STDOUT_HANDLE, "marke 1.6\r\n");
//	i2c_write_byte(I2C0_BASE_PTR, data);
//	print(STDOUT_HANDLE, "marke 1.7\r\n");
//	i2c_wait(I2C0_BASE_PTR);
//	print(STDOUT_HANDLE, "marke 1.8\r\n");
//	i2c_get_ack(I2C0_BASE_PTR);
//	print(STDOUT_HANDLE, "marke 1.9\r\n");
//	i2c_stop(I2C0_BASE_PTR);
//	print(STDOUT_HANDLE, "marke 1.10\r\n");
//	pause();

	i2c_start(I2C0_BASE_PTR);
	I2C0_D = LSM303_ADDRESS_ACCEL | I2C_WRITE;
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	
	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	
	i2c_write_byte(I2C0_BASE_PTR, data);
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	
	i2c_stop(I2C0_BASE_PTR);
	pause();
	
	return TRUE;
}

/* 
 *  initialize the accelerometer
 */
uint8 lsm303_acc_init(void)
{
	uint8 tmp;
	
	// Xen, Yen, Zen, normal mode and 1344 Hz update rate -> 10010111
	if(lsm303_acc_write_reg(LSM303_REGISTER_ACC_CTRL_REG1_A, 0x97) == FALSE) return FALSE;
	
	// select full scale range to 8G -> 00100000
	if(lsm303_acc_write_reg(LSM303_REGISTER_ACC_CTRL_REG4_A, 0x20) == FALSE) return FALSE;
	
	// check if connection is established
	if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_CTRL_REG1_A, &tmp) == FALSE) return FALSE;
	if(tmp != 0x97) return FALSE;
	
	return TRUE;
}

/* 
 *  reads the x, y, and z values and puts them in the given variables in m/s^2
 */
uint8 lsm303_acc_read(Imu_Data* values)
{
	int16 accel_x, accel_y, accel_z;
	uint8 tmp;
	
	// check if new X, Y, Z data are available
	if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_STATUS_REG_A, &tmp) == TRUE)
	{
		if((tmp & 0x08) != 0x08) return FALSE;
		
		if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_OUT_X_L_A, &tmp) == FALSE) return FALSE;
		accel_x = tmp;
		if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_OUT_X_H_A, &tmp) == FALSE) return FALSE;
		accel_x |= tmp << 8;
		accel_x >>= 2;

		if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_OUT_Y_L_A, &tmp) == FALSE) return FALSE;
		accel_y = tmp;
		if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_OUT_Y_H_A, &tmp) == FALSE) return FALSE;
		accel_y |= tmp << 8;
		accel_y >>= 2;
		
		if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_OUT_Z_L_A, &tmp) == FALSE) return FALSE;
		accel_z = tmp;
		if(lsm303_acc_read_reg(LSM303_REGISTER_ACC_OUT_Z_H_A, &tmp) == FALSE) return FALSE;
		accel_z |= tmp << 8;
		accel_z >>= 2;
		
		values->x = (float)accel_x * _lsm303Accel_MG_LSB;
		values->y = (float)accel_y * _lsm303Accel_MG_LSB;
		values->z = (float)accel_z * _lsm303Accel_MG_LSB;
			
		return TRUE;
	}
	
	return FALSE;
}

/*
 *  calculates the offset of the accelerometer values x, y, z
 */
uint8 lsm303_acc_offset_calc(int add_index)
{
	unsigned int index;
	Imu_Data values;
	
	// offset calculation
	for(index = 0; index < ACC_OFFSET_LOOPS; index++)
	{
		tooglesLED(GREEN_OB_LED);
		tooglesLED(RED_OB_LED);
		
		while(lsm303_acc_read(&values) == FALSE);
		x_acc_offset += values.x;
		y_acc_offset += values.y;
		z_acc_offset += (values.z - 1.0);    //z-value minus 1g
		
		#ifdef BLUETOOTH_ON
		send_debug_data(add_index + index);
		#endif
	}
	
	x_acc_offset /= ACC_OFFSET_LOOPS;
	y_acc_offset /= ACC_OFFSET_LOOPS;
	z_acc_offset /= ACC_OFFSET_LOOPS;
	
	return TRUE;
}

/* 
 *	reads a new set of offset compensated accelerometer values
 */ 
uint8 lsm303_acc_read_comp(Imu_Data* values)
{
	if(lsm303_acc_read(values) == TRUE)
	{
		values->x -= x_acc_offset;
		values->y -= y_acc_offset;
		values->z -= z_acc_offset;
		
		return TRUE;
	}
	
	return FALSE;
}

/*
 *	calculates the current roll and pitch angle in degrees
 */ 
void lsm303_acc_calc_current_angles(float* roll_acc, float* pitch_acc)
{
	*roll_acc = ((atan2f(acc_values.y, acc_values.z)) * RAD_DEG);
	
	if(*roll_acc > 90)
		*pitch_acc =((atan2f(acc_values.x, acc_values.y)) * RAD_DEG);
	else if(*roll_acc < -90)
		*pitch_acc = ((atan2f(acc_values.x, -acc_values.z)) * RAD_DEG);
	else 
		*pitch_acc = ((atan2f(acc_values.x, acc_values.z)) * RAD_DEG);
}


/*-----------------------------------------------------------------------------------------------------*/
/*	                                functions for the adafruit magnetometer                            */
/*-----------------------------------------------------------------------------------------------------*/

/* 
 *  reads 1 byte data from the given reg_addr and returns them
 */
uint8 lsm303_mag_read_reg(uint8 reg_addr)
{
	uint8 result;

  i2c_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, LSM303_ADDRESS_MAG | I2C_WRITE);
    
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	i2c_wait(I2C0_BASE_PTR);
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_repeated_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, LSM303_ADDRESS_MAG | I2C_READ);
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

/* 
 *  writes 1 byte data to the given reg_addr
 */
void lsm303_mag_write_reg(uint8 reg_addr, uint8 data)
{
	i2c_start(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, LSM303_ADDRESS_MAG | I2C_WRITE);
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

/* 
 *  initialize the magnetometer
 */
uint8 lsm303_mag_init(void)
{
	// set the magnetic sensor operating mode -> Continuous-conversion mode
	lsm303_mag_write_reg(LSM303_REGISTER_MAG_MR_REG_M, 0x00);

	// 220 Hz data output rate
	lsm303_mag_write_reg(LSM303_REGISTER_MAG_CRA_REG_M, 0x1C);
	
	// check if connection is established
	if(lsm303_mag_read_reg(LSM303_REGISTER_MAG_CRA_REG_M) != 0x1C) return FALSE;

	// gain configuration
	lsm303_set_mag_gain(LSM303_MAGGAIN_1_3);
	
	return TRUE;
}

/* 
 *  magnetometer gain configuration
 */
void lsm303_set_mag_gain(lsm303MagGain field_range)
{
	// configure the gain in the CRA_REG register
	lsm303_mag_write_reg(LSM303_REGISTER_MAG_CRB_REG_M, field_range);
	
	_magGain = field_range;
	
	// saves the set gain values
	switch(field_range)
  {
    case LSM303_MAGGAIN_1_3:
      _lsm303Mag_Gauss_LSB_XY = 1100;
      _lsm303Mag_Gauss_LSB_Z  = 980;
      break;
    case LSM303_MAGGAIN_1_9:
      _lsm303Mag_Gauss_LSB_XY = 855;
      _lsm303Mag_Gauss_LSB_Z  = 760;
      break;
    case LSM303_MAGGAIN_2_5:
      _lsm303Mag_Gauss_LSB_XY = 670;
      _lsm303Mag_Gauss_LSB_Z  = 600;
      break;
    case LSM303_MAGGAIN_4_0:
      _lsm303Mag_Gauss_LSB_XY = 450;
      _lsm303Mag_Gauss_LSB_Z  = 400;
      break;
    case LSM303_MAGGAIN_4_7:
      _lsm303Mag_Gauss_LSB_XY = 400;
      _lsm303Mag_Gauss_LSB_Z  = 355;
      break;
    case LSM303_MAGGAIN_5_6:
      _lsm303Mag_Gauss_LSB_XY = 330;
      _lsm303Mag_Gauss_LSB_Z  = 295;
      break;
    case LSM303_MAGGAIN_8_1:
      _lsm303Mag_Gauss_LSB_XY = 230;
      _lsm303Mag_Gauss_LSB_Z  = 205;
      break;
  } 
}

void lsm303_mag_read_poll(int16* x, int16* y, int16* z)
{
	// checks the data ready bit in SR_REG_M if new data are available
	while((lsm303_mag_read_reg(LSM303_REGISTER_MAG_SR_REG_Mg) & 0x01) != 0x1);
	
	*x  = lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_X_H_M)<<8;
	*x |= lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_X_L_M);
	
	*y  = lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Y_H_M)<<8;
	*y |= lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Y_L_M);
	
	*z  = lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Z_H_M)<<8;
	*z |= lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Z_L_M);
}

/*
 *  reads the x, y, and z values and puts them in the given variables
 */
uint8 lsm303_mag_read(int16* x, int16* y, int16* z)
{
	// checks the data ready bit in SR_REG_M if new data are available
//	if((lsm303_mag_read_reg(LSM303_REGISTER_MAG_SR_REG_Mg) & 0x01) == 0x1)
//	{
		*x  = lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_X_H_M)<<8;
		*x |= lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_X_L_M);
		
		*y  = lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Y_H_M)<<8;
		*y |= lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Y_L_M);
		
		*z  = lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Z_H_M)<<8;
		*z |= lsm303_mag_read_reg(LSM303_REGISTER_MAG_OUT_Z_L_M);
	
		return TRUE;
//	}
//	
//	return FALSE;
}

/*
 *  reads the x, y, and z values and puts them in the given variables
 *  and set a new magnetormeter gain if the sensor is saturating and the
 *  static variable _autoRangeEnabled is TRUE, if _autoRangeEnabled is
 *  FALSE the function is equal to lsm303_mag_read()
 */
uint8 lsm303_mag_clean_read(Imu_Data* values)
{
	uint8 reading_valid = FALSE;
	int16 x_t, y_t, z_t;
	
	while(!reading_valid)
	{
		if(lsm303_mag_read(&x_t, &y_t, &z_t) == FALSE) return FALSE;
		
		if(_autoRangeEnabled == FALSE)
		{
			reading_valid = TRUE;
		}
		else
		{
			if((x_t >= 2040) | (x_t <= -2040) |
				 (y_t >= 2040) | (y_t <= -2040) |
				 (z_t >= 2040) | (z_t <= -2040))
			{
				switch(_magGain)
        {
          case LSM303_MAGGAIN_5_6:
            lsm303_set_mag_gain(LSM303_MAGGAIN_8_1);
            reading_valid = FALSE;

            break;
          case LSM303_MAGGAIN_4_7:
            lsm303_set_mag_gain(LSM303_MAGGAIN_5_6);
            reading_valid = FALSE;

            break;
          case LSM303_MAGGAIN_4_0:
            lsm303_set_mag_gain(LSM303_MAGGAIN_4_7);
            reading_valid = FALSE;

            break;
          case LSM303_MAGGAIN_2_5:
            lsm303_set_mag_gain(LSM303_MAGGAIN_4_0);
            reading_valid = FALSE;
		
            break;
          case LSM303_MAGGAIN_1_9:
            lsm303_set_mag_gain(LSM303_MAGGAIN_2_5);
            reading_valid = FALSE;
			
            break;
          case LSM303_MAGGAIN_1_3:
            lsm303_set_mag_gain(LSM303_MAGGAIN_1_9);
            reading_valid = FALSE;
	
            break;
          default:
            reading_valid = TRUE;
            break;  
        }
			}
			else
			{
				reading_valid = TRUE;
			}
		}
	}
	
	// calibration routine(http://www.germersogorb.de/html/kalibrierung_des_hcm5883l.html)
	x_t += cal_offsets[0];
	y_t += cal_offsets[1];
	z_t += cal_offsets[2];
	
	values->x = cal_matrix[0][0] * x_t + cal_matrix[0][1] * y_t + cal_matrix[0][2] * z_t;
  values->y = cal_matrix[1][0] * x_t + cal_matrix[1][1] * y_t + cal_matrix[1][2] * z_t;
  values->z = cal_matrix[2][0] * x_t + cal_matrix[2][1] * y_t + cal_matrix[2][2] * z_t; 
	
	values->x /= _lsm303Mag_Gauss_LSB_XY;
	values->y /= _lsm303Mag_Gauss_LSB_XY;
	values->z /= _lsm303Mag_Gauss_LSB_Z;
	
	/*
	 *	the sensor measurements needs to be reversed to make
	 *	the sensing axes the same direction as the device body axes
   */		
	values->y = -(values->y);
	values->z = -(values->z);
	
	//printf("x: %lf   y: %lf   z: %lf\r\n", values->x, values->y, values->z);
	
	return TRUE;
}

/*
 *  calculate the heading(yaw) value from a given set of 
 *  x, y, z, roll and pitch values with a tilt compensation
 */
void get_tilt_comp_heading(float* roll, float* pitch, float* yaw)
{
	float norm;
	float mx, my, mz, mx2, my2;
	float roll_r, pitch_r;
	
	roll_r = *roll * DEG_RAD;
	pitch_r = *pitch * DEG_RAD;
	
	norm = sqrt(pow(mag_values.x, 2) + pow(mag_values.y, 2) + pow(mag_values.z, 2));
	
	mx = mag_values.x / norm;
	my = mag_values.y / norm;
	mz = mag_values.z / norm;
	
	mx2 = mx * arm_cos_f32(pitch_r) + mz * arm_sin_f32(pitch_r);
	my2 = mx * arm_sin_f32(roll_r) * arm_sin_f32(pitch_r) + my * arm_cos_f32(roll_r) - mz * arm_sin_f32(roll_r) * arm_cos_f32(pitch_r);
	
	if(mx2 > 0 && my2 >= 0) 
	{
		*yaw = atan(my2 / mx2);
	} 
	else if(mx2 < 0) 
	{
		*yaw = (PI + atan(my2 / mx2));
	} 
	else if(mx2 > 0 && my2 <= 0)
	{
		*yaw = (2 * PI + atan(my2 / mx2));
	}
	else if(mx2 == 0 && my2 < 0)
	{
		*yaw = (PI / 2.0);
	}
	else if(mx2 == 0 && my2 > 0)
	{
		*yaw = (PI + atan(my2 / mx2));
	}
	else
	{}
	
	*yaw *= RAD_DEG;
}

/*
 * reads MAG_OFFSET_LOOPS x/y/z value pairs and prints the min and max from each axis
 */
void get_calibrate_values_mag(void)
{
	int index, i;
	int16 min_x = 0, min_y = 0, min_z = 0, max_x = 0, max_y = 0, max_z = 0, mag_values[3];
	
	for(index = 0; index < MAG_OFFSET_LOOPS; index++)
	{
		for(i = 0; i < 500000; i++);
		print(EXT_DEBUG_HANDLE, ".");
		
		lsm303_mag_read_poll(&mag_values[0], &mag_values[1], &mag_values[2]);
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
	print(EXT_DEBUG_HANDLE, "\r\n");
	print(EXT_DEBUG_HANDLE, "min_x: %d   max_x: %d   \r\n", min_x, max_x);
	print(EXT_DEBUG_HANDLE, "min_y: %d   max_y: %d   \r\n", min_y, max_y);
	print(EXT_DEBUG_HANDLE, "min_z: %d   max_z: %d   \r\n", min_z, max_z);
}

/*-----------------------------------------------------------------------------------------------------*/
/*	                                functions for the adafruit gyroscope                               */
/*-----------------------------------------------------------------------------------------------------*/

/* 
 *  reads 1 byte data from the given reg_addr and returns them
 */
uint8 l3gd_gyro_read_reg(uint8 reg_addr, uint8* result)
{
	//float times[4];
	
  i2c_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, L3GD20H_GYRO_ADDRESS | I2C_WRITE);
    
	//start_timer();
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[0] = stop_timer();
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	//start_timer();
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[1] = stop_timer();
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_repeated_start(I2C0_BASE_PTR);
	i2c_write_byte(I2C0_BASE_PTR, L3GD20H_GYRO_ADDRESS | I2C_READ);
	//start_timer();
  if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[2] = stop_timer();
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_set_rx_mode(I2C0_BASE_PTR);

	i2c_give_nack(I2C0_BASE_PTR);
	*result = i2c_read_byte(I2C0_BASE_PTR);
	//start_timer();
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	//times[3] = stop_timer();
	i2c_stop(I2C0_BASE_PTR);
	*result = i2c_read_byte(I2C0_BASE_PTR);
	pause();
	
	//printf("G %lf %lf %lf %lf\r\n", times[0], times[1], times[2], times[3]);

	return TRUE;
}

/* 
 *  writes 1 byte data to the given reg_addr
 */
uint8 l3gd_gyro_write_reg(uint8 reg_addr, uint8 data)
{
	i2c_start(I2C0_BASE_PTR);

  i2c_write_byte(I2C0_BASE_PTR, L3GD20H_GYRO_ADDRESS | I2C_WRITE);
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, reg_addr);
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_write_byte(I2C0_BASE_PTR, data);
	if(i2c_wait(I2C0_BASE_PTR) == FALSE) return FALSE;
	i2c_get_ack(I2C0_BASE_PTR);

	i2c_stop(I2C0_BASE_PTR);
	pause();
	
	return TRUE;
}

/* 
 *  initialize the gyroscope
 */
uint8 l3gd_gyro_init(void)
{
	uint8 tmp;
	uint32 index = 1000;
	
	// check if gyro is available
	do
	{
		if(l3gd_gyro_read_reg(GYRO_REGISTER_WHO_AM_I, &tmp) == FALSE || index == 0) return FALSE;
		index--;
	}while(tmp != L3GD20H_ID);
	
	/*
	 *  enable X and Y axis, power mode -> normal,
	 *  output data rate -> 800 Hz without cut off frequency
	 *  => 11101111
	 */ 
	if(l3gd_gyro_write_reg(GYRO_REGISTER_CTRL_REG1, 0xEF) == FALSE) return FALSE;

  // Full scale selection to 500 dps
	if(l3gd_gyro_write_reg(GYRO_REGISTER_CTRL_REG4, 0x10) == FALSE) return FALSE;
	
	return TRUE;
}

/*
 *  reads the x, y, and z value and puts them in the given variables
 */ 
uint8 l3gd_gyro_read(Imu_Data* values)
{
	int16 gyro_x, gyro_y, gyro_z;
	uint8 tmp;
	
	// read status register and check if data available
	if((l3gd_gyro_read_reg(GYRO_REGISTER_STATUS_REG, &tmp) == TRUE) && ((tmp & 0x3) == 0x3))
	{
		if(l3gd_gyro_read_reg(GYRO_REGISTER_OUT_X_L, &tmp) == FALSE) return FALSE;
		gyro_x = tmp;
		if(l3gd_gyro_read_reg(GYRO_REGISTER_OUT_X_H, &tmp) == FALSE) return FALSE;
		gyro_x |= tmp << 8;
		
		if(l3gd_gyro_read_reg(GYRO_REGISTER_OUT_Y_L, &tmp) == FALSE) return FALSE;
		gyro_y = tmp;
		if(l3gd_gyro_read_reg(GYRO_REGISTER_OUT_Y_H, &tmp) == FALSE) return FALSE;
		gyro_y |= tmp << 8;
	
		if(l3gd_gyro_read_reg(GYRO_REGISTER_OUT_Z_L, &tmp) == FALSE) return FALSE;
		gyro_z = tmp;
		if(l3gd_gyro_read_reg(GYRO_REGISTER_OUT_Z_H, &tmp) == FALSE) return FALSE;
		gyro_z |= tmp << 8;
		
		values->x = gyro_x;
		values->y = -gyro_y; //workaround for wrong axis sign
		values->z = -gyro_z; //workaround for wrong axis sign
	
		return TRUE;
	}
	
	return FALSE;
}

/* 
 *  read a new set of offset compensated gyro values
 */ 
uint8 l3gd_gyro_read_comp(Imu_Data* values)
{
	if(l3gd_gyro_read(values) == TRUE)
	{
		values->x -= x_gyro_offset;
		values->y -= y_gyro_offset;
		values->z -= z_gyro_offset;
		
		values->x *= GYRO_SENSITIVITY_500DPS;
		values->y *= GYRO_SENSITIVITY_500DPS;
		values->z *= GYRO_SENSITIVITY_500DPS;
		
		return TRUE;
	}
	
	return FALSE;
}

/* 
 *  calculates the offset of the gyro values
 */ 
uint8 l3gd_gyro_offset_calc(int add_index)
{
	unsigned int index;
	Imu_Data values;
	
	// offset calculation
	for(index = 0; index < GYRO_OFFSET_LOOPS; index++)
	{
		tooglesLED(GREEN_OB_LED);
		tooglesLED(RED_OB_LED);
		
		while(l3gd_gyro_read(&values) == FALSE);
		x_gyro_offset += values.x;
		y_gyro_offset += values.y;
		z_gyro_offset += values.z;
		
		#ifdef BLUETOOTH_ON
		send_debug_data(add_index + index);
		#endif
	}
	
	x_gyro_offset /= GYRO_OFFSET_LOOPS;
	y_gyro_offset /= GYRO_OFFSET_LOOPS;
	z_gyro_offset /= GYRO_OFFSET_LOOPS;
	
	return TRUE;
}

/*
 *	calculates the current roll and pitch angle in degrees
 */ 
void l3gd_gyro_calc_current_angles(float* roll_angle, float* pitch_angle, float* yaw_angle)
{
	gyro_values.x *= DELTA_T;
	gyro_values.y *= DELTA_T;
	gyro_values.z *= DELTA_T;
	
	*roll_angle += gyro_values.x;
	*pitch_angle += gyro_values.y;
	*yaw_angle += gyro_values.z;
}

/*-----------------------------------------------------------------------------------------------------*/
/*	                             functions for the adafruit barometer                                  */
/*-----------------------------------------------------------------------------------------------------*/

/* 
 *  initialize the barometer BMP085
 */
uint8 bmp085_baro_init(void)
{
	uint8 tmp;
	uint32 index = 1000;
	
	// check if baro is available
	do
	{
		if(l3gd_gyro_read_reg(GYRO_REGISTER_WHO_AM_I, &tmp) == FALSE || index == 0) return FALSE;
	}while(tmp != L3GD20H_ID);
	
	/*
	 *  enable X and Y axis, power mode -> normal,
	 *  output data rate -> 800 Hz without cut off frequency
	 *  => 11101111
	 */ 
	if(l3gd_gyro_write_reg(GYRO_REGISTER_CTRL_REG1, 0xEF) == FALSE) return FALSE;

  // Full scale selection to 500 dps
	if(l3gd_gyro_write_reg(GYRO_REGISTER_CTRL_REG4, 0x10) == FALSE) return FALSE;
	
	return TRUE;
}


/*-----------------------------------------------------------------------------------------------------*/
/*	                                functions for the adafruit modul                                   */
/*-----------------------------------------------------------------------------------------------------*/

/*
 *	a wrapper function for a "complete" reading from the adafruit modul
 */
void read_sensor_data(void)
{	
	if(lsm303_acc_read_comp(&acc_values) == TRUE)
	{
		acc_low_pass_filter(&acc_values);
	}
//	else
//		printf("Err acc read\r\n");
	
	if(l3gd_gyro_read_comp(&gyro_values) == TRUE)
	{
		gyro_low_pass_filter(&gyro_values);
	}
//	else
//		printf("Err gyr read\r\n");
}

void start_calibration(void)
{
	clearLED(RED_OB_LED);
	lsm303_acc_offset_calc(0);
	l3gd_gyro_offset_calc(ACC_OFFSET_LOOPS);
	
	#ifdef BLUETOOTH_ON
	send_offset_value_data();
	#endif
		
	clearLED(GREEN_OB_LED);
	setLED(RED_OB_LED);
}




