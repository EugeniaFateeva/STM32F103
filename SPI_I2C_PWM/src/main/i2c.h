#include "mcu_support_package/inc/stm32f10x.h"

#ifndef __I2C_H
#define __I2C_H

#define I2C_MASTER
//#define I2C_SLAVE

#define I2C_SLAVE_ADDRESS	0xAA //написан рандомный адрес

void I2C1_init(void);
void I2C2_init(void);
void I2C_start(void);
void I2C_stop(void);
void I2C_slave_selection(uint8_t slaveAddress, uint8_t direction);
void I2C_write(uint8_t address, uint8_t data);
uint8_t i2c_read(uint8_t address);

#endif
