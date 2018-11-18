#ifndef LPTMR_H
#define LPTMR_H

// possible prescaler/glitchfilter values
#define PRE_2 			0
#define PRE_4 			1
#define PRE_8 			2
#define PRE_16			3
#define PRE_32			4
#define PRE_64			5
#define PRE_128			6
#define PRE_256			7
#define PRE_512			8
#define PRE_1024		9
#define PRE_2048		10
#define PRE_4096		11
#define PRE_8192		12
#define PRE_16384		13
#define PRE_32768		14
#define PRE_65536		15


// 32kHz clock sources for the ERCLK32K clock(SIM_SOPT1 reg, manual page 124 and 204)
#define OSC32KCLK 0 
#define RTC_CLKIN 2 
#define LPO_1KHZ  3

// low power timer clock source
#define LPTMR_USE_IRCLK 0 
#define LPTMR_USE_LPOCLK 1
#define LPTMR_USE_ERCLK32 2 
#define LPTMR_USE_OSCERCLK 3

void lptmr_init(int cnt_mode);

#endif
