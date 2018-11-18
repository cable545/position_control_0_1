#include "main.h"

void i2c_set_tx_mode(I2C_MemMapPtr p)
{
	p->C1 |= I2C_C1_TX_MASK;
}

void i2c_set_rx_mode(I2C_MemMapPtr p)
{
	p->C1 &= ~I2C_C1_TX_MASK;
}

void i2c_set_slave_mode(I2C_MemMapPtr p)
{
	p->C1  &= ~I2C_C1_MST_MASK;
}

void i2c_set_master_mode(I2C_MemMapPtr p)
{
	p->C1  |=  I2C_C1_MST_MASK;
}

// i2c general

void i2c_give_nack(I2C_MemMapPtr p)
{
	p->C1 |= I2C_C1_TXAK_MASK;
}

void i2c_give_ack(I2C_MemMapPtr p)
{
	p->C1 &= ~I2C_C1_TXAK_MASK;
}

void i2c_repeated_start(I2C_MemMapPtr p)
{
	p->C1 |= 0x04;
}

void i2c_write_byte(I2C_MemMapPtr p, uint8 data)
{
	p->D = data;
}

uint8 i2c_read_byte(I2C_MemMapPtr p)
{
	return p->D;
}

void i2c_start(I2C_MemMapPtr p)
{
	i2c_set_master_mode(p);
	i2c_set_tx_mode(p);
}

void i2c_stop(I2C_MemMapPtr p)
{
	i2c_set_slave_mode(p);
	i2c_set_rx_mode(p);
}

uint8 i2c_wait(I2C_MemMapPtr p)
{
	uint32 timout_cnt = 2500;
	
	// wait flag
	while((p->S & I2C_S_IICIF_MASK) == 0 && timout_cnt != 0) {timout_cnt--;}
		
	if(timout_cnt == 0)
	{
		i2c_restore(p);
		return FALSE;
	}
	
	// clear flag
	p->S |= I2C_S_IICIF_MASK;
	
	return TRUE;
}

uint16 i2c_get_ack(I2C_MemMapPtr p)
{
	if((p->S & I2C_S_RXAK_MASK) == 0)
		return TRUE;
	else
		return FALSE;
}

void i2c_restore(I2C_MemMapPtr p)
{
	i2c_stop(p);
	i2c_deinit(p);
	i2c_init_adafruit_imu();
}

void i2c_deinit(I2C_MemMapPtr p)
{
	// disable I2C module
  p->C1 = 0x00;
}

void pause(void)
{
  for(int n = 0; n < 40; n++);
		__asm("nop");	
}

void cost_pause(uint32 iterations)
{
  for(int n = 0; n < iterations; n++);
		__asm("nop");	
}

/* initialize the i2c modul for the accelerometer on the 
 * kinetis board
 */
void i2c_init_acc(void)
{
	I2C_MemMapPtr p = I2C0_BASE_PTR;
	
	//set the pins 24 and 25 from Port E to I2C0_SCL and I2C0_SDA
	PORTE_PCR24 = PORT_PCR_MUX(5);
	PORTE_PCR25 = PORT_PCR_MUX(5);
     
	/* i2c frequency divider register 
	 * Reference manual page 715 and 730
	 *   7  6   5  4  3  2  1  0
	 *  ---------------------------
	 * |  mul |       ICR          |
	 *  ---------------------------
	 * 24 MHz Bus Clock
	 * I2C Baud Rate have to be 400 kHz
	 * I2C Baud Rate = Bus Clock / (mul x SCL divider)
	 * mul = 2 => 01
	 * ICR = 0x05 => SCL divider = 30
	 * 400000 = 24000000 / (2 x 30)
	 *   7  6   5  4  3  2  1  0
	 *  ---------------------------
	 * | 0  1 | 0  0  0  1  0  1   |
	 *  ---------------------------
	 * 0x45
	*/
	p->F  = 0x45; // baudrate
	//i2c control register
	p->C1 = 0x80; // enable I2C module
}

void i2c_init_mag(void)
{
	I2C_MemMapPtr p = I2C0_BASE_PTR;
	
	//enables the clock for the i2c0 modul
	SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
	
	// configure GPIO for I2C function
	PORTE_PCR24 = PORT_PCR_MUX(5);
	PORTE_PCR25 = PORT_PCR_MUX(5);
	
	p->F  = 0x45; // baudrate
	p->C1 = 0x80; // enable IIC
}

void i2c_deinit_acc_mag(void)
{
		I2C_MemMapPtr p = I2C0_BASE_PTR;
		
		// disable I2C module
    p->C1 = 0x00;
    //disables the clock for the i2c0 modul
    SIM_SCGC4 &= ~SIM_SCGC4_I2C0_MASK;
}

void i2c_init_ex_gyro(void)
{
	I2C_MemMapPtr p = I2C0_BASE_PTR;

	//enables the clock for the i2c0 modul
	SIM_SCGC4 |= SIM_SCGC4_I2C0_MASK;
	//enables the clock for Port E
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;;

	//set the pins 18 and 19 from Port E to I2C0_SDA and I2C0_SCL
	PORTE_PCR18 = PORT_PCR_MUX(4); // I2C0_SDA
	PORTE_PCR19 = PORT_PCR_MUX(4); // I2C0_SCL

	/* i2c frequency divider register 
	 * Reference manual page 715 and 730
	 *   7  6   5  4  3  2  1  0
	 *  -------------------------
	 * |  mul |       ICR        |
	 *  -------------------------
	 * 24 MHz Bus Clock
	 * I2C Baud Rate have to be 400 kHz
	 * I2C Baud Rate = Bus Clock / (mul x SCL divider)
	 * mul = 2 => 01
	 * ICR = 0x05 => SCL divider = 30
	 * 400000 = 24000000 / (2 x 30)
	 *   7  6   5  4  3  2  1  0
	 *  -------------------------
	 * | 0  1 | 0  0  0  1  0  1 |
	 *  -------------------------
	 * 0x45
	*/
	p->F  = 0x45; // baudrate
	//i2c control register
	p->C1 = 0x80; // enable I2C module
}

/* 
 * initialize the i2c modul for the communication with the external adafruit imu
 */
void i2c_init_adafruit_imu(void)
{
	I2C_MemMapPtr p = I2C0_BASE_PTR;
	
	//set the pins 18 and 19 from Port E to I2C0_SCL and I2C0_SDA
	PORTE_PCR18 = PORT_PCR_MUX(4); // I2C0_SDA
	PORTE_PCR19 = PORT_PCR_MUX(4); // I2C0_SCL
     
	/* i2c frequency divider register 
	 * Reference manual page 715 and 730
	 *   7  6   5  4  3  2  1  0
	 *  ---------------------------
	 * |  mul |       ICR          |
	 *  ---------------------------
	 * 24 MHz Bus Clock
	 * I2C Baud Rate have to be 400 kHz
	 * I2C Baud Rate = Bus Clock / (mul x SCL divider)
	 * mul = 2 => 01
	 * ICR = 0x05 => SCL divider = 30
	 * 400000 = 24000000 / (2 x 30)
	 *   7  6   5  4  3  2  1  0
	 *  ---------------------------
	 * | 0  1 | 0  0  0  1  0  1   |
	 *  ---------------------------
	 * 0x45
	*/
	p->F  = 0x45; // i2c clock rate
	//p->F  = 0x01; // i2c clock rate

	//i2c control register
	p->C1 = 0x80; // enable I2C module
}
