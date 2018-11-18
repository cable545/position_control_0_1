#ifndef SYS_TICK_H
#define SYS_TICK_H

#define SYSTICK_FREQUENCY	1000
#define SENSOR_READ_FREQUENCY 100
#define SENSOR_READ_TICKS 158400

void Delay_mS(unsigned int TicksIn_mS);
void InitSysTick(void);
void InitSysTickForSensorFusion(void);

extern volatile unsigned int Ticker[4];

#endif
