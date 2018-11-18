#include "main.h"

int mcg_clk_hz;
int mcg_clk_khz;
int core_clk_khz;

uint8 sysinit(void)
{
	//computer operation properly control register, disables the watchdog timer
	SIM_COPC = 0x0;
	
	// enable all of the port clocks
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
						| SIM_SCGC5_PORTB_MASK
						| SIM_SCGC5_PORTC_MASK
						| SIM_SCGC5_PORTD_MASK
						| SIM_SCGC5_PORTE_MASK );
	
	initBoardLED(GREEN_OB_LED | RED_OB_LED);
	setLED(RED_OB_LED);
	
	// set the system dividers => OUTDIV1 = divide by 1, OUTDIV4 = divide by 2
	SIM_CLKDIV1 = (0 | SIM_CLKDIV1_OUTDIV1(OUTDIV1_1) | SIM_CLKDIV1_OUTDIV4(OUTDIV4_2));
	
	// 8 MHZ crystal ckl a divider of 4 => is the PLL Ref clk and a multiply of 24 to get the core clk of 48 Mhz
	mcg_clk_hz = pll_init(CLK0_FREQ_HZ, LOW_POWER, CLK0_TYPE, PLL0_PRDIV, PLL0_VDIV, MCGOUT);
	mcg_clk_khz = mcg_clk_hz / 1000;
	core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
	
	// MCGPLLCLK clock with fixed divide by two
	SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; 						
	
	//enables the clock for the i2c0 modul
	SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
	
	// uart0 module initialization
	uart0_init(1382400, mcg_clk_hz / 2);
	
	#ifdef BLUETOOTH_ON
	// uart1 module initialization(only for tests)
	uart1_init_bluetooth(115200);
	#endif
	// uart2 module initialization
	uart2_init_ext_debug(115200);
	
	// inits the tpm1 for timing measurements
	init_timer();
	
	// inits the gpio ports for the external buzzer
	init_buzzer();

	printf("\r\ninitializing application hardware\r\n");
	
	// inits adafruit modul
	i2c_init_adafruit_imu();
	
	printf("init imu ready\r\n");
	
	// inits adafruit acc
	if(lsm303_acc_init() == FALSE)
	{
		printf("acc init fehlgeschlagen\r\n");
		return FALSE;
	}
	
	printf("init acc ready\r\n");
	
	// inits adafruit mag
//	if(lsm303_mag_init() == FALSE)
//	{
//		print(EXT_DEBUG_HANDLE, "mag init fehlgeschlagen\r\n");
//		return FALSE;
//	}
	
//	printf("init mag ready\r\n");
	
	// inits adafruit gyro
	if(l3gd_gyro_init() == FALSE)
	{
		printf("gyro init fehlgeschlagen\r\n");
		return FALSE;
	}
	
	printf("init gyro ready\r\n");
	
	return TRUE;
}
