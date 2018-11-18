#ifndef I2C_H
#define	I2C_H

#define I2C_READ  1
#define I2C_WRITE 0

void i2c_set_tx_mode(I2C_MemMapPtr p);
void i2c_set_rx_mode(I2C_MemMapPtr p);
void i2c_set_slave_mode(I2C_MemMapPtr p);
void i2c_set_master_mode(I2C_MemMapPtr p);
void i2c_give_nack(I2C_MemMapPtr p);
void i2c_give_ack(I2C_MemMapPtr p);
void i2c_repeated_start(I2C_MemMapPtr p);
void i2c_write_byte(I2C_MemMapPtr p, uint8 data);
uint8 i2c_read_byte(I2C_MemMapPtr p);
void i2c_start(I2C_MemMapPtr p);
void i2c_stop(I2C_MemMapPtr p);
uint8 i2c_wait(I2C_MemMapPtr p);
uint16 i2c_get_ack(I2C_MemMapPtr p);
void i2c_restore(I2C_MemMapPtr p);
void pause(void);
void cost_pause(uint32 iterations);
void i2c_deinit(I2C_MemMapPtr p);

void i2c_init_acc(void);
void i2c_init_mag(void);
void i2c_init_acc_mag(void);
void i2c_deinit_acc_mag(void);

void i2c_init_ex_gyro(void);

void i2c_init_adafruit_imu(void);

#endif
