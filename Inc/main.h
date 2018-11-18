#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdarg.h>	
#include <arm_math.h>
#include "MKL46Z4.h"
#include "dataTypes.h"
#include "extended_math.h"

#include "sysinit.h"
#include "led.h"
#include "mcg.h"
#include "tpm.h"
#include "sysTick.h"
#include "interrupt.h"
#include "uart.h"
#include "i2c.h"
#include "dma.h"
#include "receiver.h"
#include "adafruit_modul.h"
#include "kalman_filter.h"
#include "sensor_fusion.h"
#include "controller.h"
#include "debug.h"
#include "pwm.h"
#include "bluetooth_connection.h"
#include "filter.h"
#include "buzzer.h"

// static defines for pin ALTs(manual page 171)
#define PIN_ALT_0							0
#define PIN_ALT_1							1
#define PIN_ALT_2							2
#define PIN_ALT_3							3
#define PIN_ALT_4							4
#define PIN_ALT_5							5
#define PIN_ALT_6							6
#define PIN_ALT_7							7

#define REC_TO_MOT_PWM				8

#define TRUE 	1
#define FALSE 0

#define NULL_VALUE						0

//#define DELTA_T 0.00534//ohne param funktion und printf 0.00283

// states for the state machine
typedef enum
{
	init_hardware_state = 0,
	init_software_state = 1,
	security_state = 2,
	operating_state = 3,
	error_state = 4
} main_loop_states;

void init_timer(void);
void start_timer(void);
float get_millis(void);
float stop_timer(void);
int map(int x, int in_min, int in_max, int out_min, int out_max);

void go_in_sec_state(void);
void go_in_op_state(void);

#endif
