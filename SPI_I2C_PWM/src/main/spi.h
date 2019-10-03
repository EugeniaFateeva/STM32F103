#include "mcu_support_package/inc/stm32f10x.h"

#ifndef __SPI_H
#define __SPI_H

#define SPI_MASTER
//#define SPI_SLAVE

void init_SPI1(void);
void init_SPI2(void);

void spi_send(uint8_t data);

#endif

