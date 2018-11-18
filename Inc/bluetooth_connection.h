#ifndef BLUETOOTH_CONNECTION_H
#define BLUETOOTH_CONNECTION_H

#define BLUETOOTH_ON

#define INIT_DATA_BUFFER_LENGTH 							49
#define DEBUG_DATA_BUFFER_LENGTH							32
#define SENSOR_DATA_BUFFER_LENGTH							44
#define	OFFSET_DATA_BUFFER_LENGTH							28

#define CONNECT_MESSAGE_ID									0x01
#define CONNECT_MESSAGE_RESPONSE_ID					0x02
#define DATA_ROLL_MESSAGE_ID								0x03
#define DATA_PITCH_MESSAGE_ID								0x04
#define DATA_YAW_MESSAGE_ID									0x05
#define DATA_ALL_MESSAGE_ID									0x06
#define DATA_ROLL_PITCH_MESSAGE_ID					0x07
#define DATA_ROLL_YAW_MESSAGE_ID						0x08
#define DATA_YAW_PITCH_MESSAGE_ID						0x09
#define DEBUG_DATA_START_SIGNAL_MESSAGE_ID	0x0A
#define DISCONNECT_MESSAGE_ID								0x0B
#define START_CALIB_MESSAGE_ID							0x0E
#define DEBUG_DATA_MESSAGE_ID								0x1234
#define OFFSET_DATA_MESSAGE_ID							0x3234
#define SENSOR_DATA_MESSAGE_ID							0x5234

void compute_bluetooth_data(void);
void cp_buffer_to_var(char* buffer, float* kp_angle, float* ki_angle, float* kp_rate, float* ki_rate);
void cp_var_to_buffer(char* buffer, float* kp_angle, float* ki_angle, float* kp_rate, float* ki_rate);
void send_initial_controller_data(void);
void act_bluetooth_receive_int(void);
void deact_bluetooth_receive_int(void);
void send_data(char* data, int data_length);
void send_debug_data(int calib_value);
void init_bluetooth_data_array(void);
void send_offset_value_data(void);


#endif
