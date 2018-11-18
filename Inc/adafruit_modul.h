#ifndef ADAFRUIT_MODUL_H
#define	ADAFRUIT_MODUL_H

typedef struct
{
	float x;
	float y;
	float z;
} Imu_Data;

/* Constants */
#define SENSORS_DECLINATION								61.38/1000.0
#define SENSORS_GRAVITY_EARTH             (9.80665F)              /**< Earth's gravity in m/s^2 */
#define SENSORS_GRAVITY_MOON              (1.6F)                  /**< The moon's gravity in m/s^2 */
#define SENSORS_GRAVITY_SUN               (275.0F)                /**< The sun's gravity in m/s^2 */
#define SENSORS_GRAVITY_STANDARD          (SENSORS_GRAVITY_EARTH)
#define SENSORS_MAGFIELD_EARTH_MAX        (60.0F)                 /**< Maximum magnetic field on Earth's surface */
#define SENSORS_MAGFIELD_EARTH_MIN        (30.0F)                 /**< Minimum magnetic field on Earth's surface */
#define SENSORS_PRESSURE_SEALEVELHPA      (1013.25F)              /**< Average sea level pressure is 1013.25 hPa */
#define SENSORS_DPS_TO_RADS               (0.017453293F)          /**< Degrees/s to rad/s multiplier */
#define SENSORS_GAUSS_TO_MICROTESLA       (100)                   /**< Gauss to micro-Tesla multiplier */
#define GYRO_OFFSET_LOOPS									20000
#define ACC_OFFSET_LOOPS									20000
#define MAG_OFFSET_LOOPS									1000
#define VARIANCE_CALC_LOOPS								100

#define MAG_X_OFFSET											(15.5F)
#define MAG_Y_OFFSET											(-78.5F)
#define MAG_Z_OFFSET											(-65.5F)

#define MAG_MIN_X 												(-525.0F)
#define MAG_MAX_X  												(537.0F)
#define MAG_MIN_Y 												(-611.0F)
#define MAG_MAX_Y  												(473.0F)
#define MAG_MIN_Z 												(-564.0F)
#define MAG_MAX_Z  												(412.0F)

/*=========================================================================
 I2C ADDRESS/BITS AND SETTINGS
 -----------------------------------------------------------------------*/
#define LSM303_ADDRESS_ACCEL          (0x19 << 1)         // 0011001x
#define LSM303_ADDRESS_MAG            (0x1E << 1)         // 0011110x
#define L3GD20H_GYRO_ADDRESS 					(0x6B << 1)					// 1101011x

#define L3GD20_POLL_TIMEOUT      (100)         // Maximum number of read attempts
#define L3GD20H_ID               0xD7					 // Device ID, stored in the device identification register
#define GYRO_SENSITIVITY_250DPS  (0.00875F)    // Roughly 22/256 for fixed point match
#define GYRO_SENSITIVITY_500DPS  (0.0175F)     // Roughly 45/256
#define GYRO_SENSITIVITY_2000DPS (0.070F)      // Roughly 18/256
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
    typedef enum
    {                                                     // DEFAULT    TYPE
      LSM303_REGISTER_ACC_CTRL_REG1_A         	= 0x20,   // 00000111   rw
      LSM303_REGISTER_ACC_CTRL_REG2_A         	= 0x21,   // 00000000   rw
      LSM303_REGISTER_ACC_CTRL_REG3_A         	= 0x22,   // 00000000   rw
      LSM303_REGISTER_ACC_CTRL_REG4_A         	= 0x23,   // 00000000   rw
      LSM303_REGISTER_ACC_CTRL_REG5_A         	= 0x24,   // 00000000   rw
      LSM303_REGISTER_ACC_CTRL_REG6_A         	= 0x25,   // 00000000   rw
      LSM303_REGISTER_ACC_REFERENCE_A         	= 0x26,   // 00000000   r
      LSM303_REGISTER_ACC_STATUS_REG_A        	= 0x27,   // 00000000   r
      LSM303_REGISTER_ACC_OUT_X_L_A           	= 0x28,
      LSM303_REGISTER_ACC_OUT_X_H_A           	= 0x29,
      LSM303_REGISTER_ACC_OUT_Y_L_A           	= 0x2A,
      LSM303_REGISTER_ACC_OUT_Y_H_A           	= 0x2B,
      LSM303_REGISTER_ACC_OUT_Z_L_A           	= 0x2C,
      LSM303_REGISTER_ACC_OUT_Z_H_A           	= 0x2D,
      LSM303_REGISTER_ACC_FIFO_CTRL_REG_A     	= 0x2E,
      LSM303_REGISTER_ACC_FIFO_SRC_REG_A      	= 0x2F,
      LSM303_REGISTER_ACC_INT1_CFG_A          	= 0x30,
      LSM303_REGISTER_ACC_INT1_SOURCE_A       	= 0x31,
      LSM303_REGISTER_ACC_INT1_THS_A          	= 0x32,
      LSM303_REGISTER_ACC_INT1_DURATION_A     	= 0x33,
      LSM303_REGISTER_ACC_INT2_CFG_A          	= 0x34,
      LSM303_REGISTER_ACC_INT2_SOURCE_A       	= 0x35,
      LSM303_REGISTER_ACC_INT2_THS_A          	= 0x36,
      LSM303_REGISTER_ACC_INT2_DURATION_A     	= 0x37,
      LSM303_REGISTER_ACC_CLICK_CFG_A         	= 0x38,
      LSM303_REGISTER_ACC_CLICK_SRC_A         	= 0x39,
      LSM303_REGISTER_ACC_CLICK_THS_A         	= 0x3A,
      LSM303_REGISTER_ACC_TIME_LIMIT_A        	= 0x3B,
      LSM303_REGISTER_ACC_TIME_LATENCY_A      	= 0x3C,
      LSM303_REGISTER_ACC_TIME_WINDOW_A       	= 0x3D
    } lsm303AccelRegisters_t;
    
    typedef enum
    {
      LSM303_REGISTER_MAG_CRA_REG_M             = 0x00,
      LSM303_REGISTER_MAG_CRB_REG_M             = 0x01,
      LSM303_REGISTER_MAG_MR_REG_M              = 0x02,
      LSM303_REGISTER_MAG_OUT_X_H_M             = 0x03,
      LSM303_REGISTER_MAG_OUT_X_L_M             = 0x04,
      LSM303_REGISTER_MAG_OUT_Z_H_M             = 0x05,
      LSM303_REGISTER_MAG_OUT_Z_L_M             = 0x06,
      LSM303_REGISTER_MAG_OUT_Y_H_M             = 0x07,
      LSM303_REGISTER_MAG_OUT_Y_L_M             = 0x08,
      LSM303_REGISTER_MAG_SR_REG_Mg             = 0x09,
      LSM303_REGISTER_MAG_IRA_REG_M             = 0x0A,
      LSM303_REGISTER_MAG_IRB_REG_M             = 0x0B,
      LSM303_REGISTER_MAG_IRC_REG_M             = 0x0C,
      LSM303_REGISTER_MAG_TEMP_OUT_H_M          = 0x31,
      LSM303_REGISTER_MAG_TEMP_OUT_L_M          = 0x32
    } lsm303MagRegisters_t;
		
		typedef enum
    {
      GYRO_REGISTER_WHO_AM_I           				 	= 0x0F,   // 11010100   r
      GYRO_REGISTER_CTRL_REG1          				 	= 0x20,   // 00000111   rw
      GYRO_REGISTER_CTRL_REG2          				 	= 0x21,   // 00000000   rw
      GYRO_REGISTER_CTRL_REG3          				 	= 0x22,   // 00000000   rw
      GYRO_REGISTER_CTRL_REG4          				 	= 0x23,   // 00000000   rw
      GYRO_REGISTER_CTRL_REG5          				 	= 0x24,   // 00000000   rw
      GYRO_REGISTER_REFERENCE          				 	= 0x25,   // 00000000   rw
      GYRO_REGISTER_OUT_TEMP           				 	= 0x26,   //            r
      GYRO_REGISTER_STATUS_REG         				 	= 0x27,   //            r
      GYRO_REGISTER_OUT_X_L            				 	= 0x28,   //            r
      GYRO_REGISTER_OUT_X_H            				 	= 0x29,   //            r
      GYRO_REGISTER_OUT_Y_L            				 	= 0x2A,   //            r
      GYRO_REGISTER_OUT_Y_H            				 	= 0x2B,   //            r
      GYRO_REGISTER_OUT_Z_L            				 	= 0x2C,   //            r
      GYRO_REGISTER_OUT_Z_H            				 	= 0x2D,   //            r
      GYRO_REGISTER_FIFO_CTRL_REG      				 	= 0x2E,   // 00000000   rw
      GYRO_REGISTER_FIFO_SRC_REG       				 	= 0x2F,   //            r
      GYRO_REGISTER_INT1_CFG           				 	= 0x30,   // 00000000   rw
      GYRO_REGISTER_INT1_SRC           				 	= 0x31,   //            r
      GYRO_REGISTER_TSH_XH             				 	= 0x32,   // 00000000   rw
      GYRO_REGISTER_TSH_XL             				 	= 0x33,   // 00000000   rw
      GYRO_REGISTER_TSH_YH             				 	= 0x34,   // 00000000   rw
      GYRO_REGISTER_TSH_YL             				 	= 0x35,   // 00000000   rw
      GYRO_REGISTER_TSH_ZH             				 	= 0x36,   // 00000000   rw
      GYRO_REGISTER_TSH_ZL             				 	= 0x37,   // 00000000   rw
      GYRO_REGISTER_INT1_DURATION      				 	= 0x38    // 00000000   rw
    } gyroRegisters_t;    
/*=========================================================================*/

/*=========================================================================
    MAGNETOMETER GAIN SETTINGS
    -----------------------------------------------------------------------*/
    typedef enum
    {
      LSM303_MAGGAIN_1_3                        = 0x20,  // +/- 1.3
      LSM303_MAGGAIN_1_9                        = 0x40,  // +/- 1.9
      LSM303_MAGGAIN_2_5                        = 0x60,  // +/- 2.5
      LSM303_MAGGAIN_4_0                        = 0x80,  // +/- 4.0
      LSM303_MAGGAIN_4_7                        = 0xA0,  // +/- 4.7
      LSM303_MAGGAIN_5_6                        = 0xC0,  // +/- 5.6
      LSM303_MAGGAIN_8_1                        = 0xE0   // +/- 8.1
    } lsm303MagGain;	
/*=========================================================================*/
		
/*=========================================================================
    MAGNETOMETER UPDATE RATE SETTINGS
    -----------------------------------------------------------------------*/
    typedef enum
    {
      LSM303_MAGRATE_0_7                        = 0x00,  // 0.75 Hz
      LSM303_MAGRATE_1_5                        = 0x01,  // 1.5 Hz
      LSM303_MAGRATE_3_0                        = 0x62,  // 3.0 Hz
      LSM303_MAGRATE_7_5                        = 0x03,  // 7.5 Hz
      LSM303_MAGRATE_15                         = 0x04,  // 15 Hz
      LSM303_MAGRATE_30                         = 0x05,  // 30 Hz
      LSM303_MAGRATE_75                         = 0x06,  // 75 Hz
      LSM303_MAGRATE_220                        = 0x07   // 200 Hz
    } lsm303MagRate;	
/*=========================================================================*/

uint8 lsm303_acc_read_reg(uint8 reg_addr, uint8* result);
uint8 lsm303_acc_write_reg(uint8 reg_addr, uint8 data);
uint8 lsm303_acc_init(void);
uint8 lsm303_acc_offset_calc(int add_index);
uint8 lsm303_acc_read(Imu_Data* values);
uint8 lsm303_acc_read_comp(Imu_Data* values);
void lsm303_acc_calc_current_angles(float* roll_acc, float* pitch_acc);
		
uint8 lsm303_mag_read_reg(uint8 reg_addr);
void lsm303_mag_write_reg(uint8 reg_addr, uint8 data);
uint8 lsm303_mag_init(void);
void lsm303_set_mag_gain(lsm303MagGain field_range);
uint8 lsm303_mag_read(int16* x, int16* y, int16* z);
uint8 lsm303_mag_clean_read(Imu_Data* values);
void lsm303_mag_read_poll(int16* x, int16* y, int16* z);
void get_calibrate_values_mag(void);

float get_heading(void);
void get_tilt_comp_heading(float* roll, float* pitch, float* yaw);
		
uint8 l3gd_gyro_read_reg(uint8 reg_addr, uint8* result);
uint8 l3gd_gyro_write_reg(uint8 reg_addr, uint8 data);
uint8 l3gd_gyro_init(void);
uint8 l3gd_gyro_read(Imu_Data* values);
uint8 l3gd_gyro_offset_calc(int add_index);
uint8 l3gd_gyro_read_comp(Imu_Data* values);
void l3gd_gyro_calc_current_angles(float* roll_angle, float* pitch_angle, float* yaw_angle);

void read_sensor_data(void);
void start_calibration(void);

#endif
