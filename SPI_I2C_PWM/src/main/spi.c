#include "spi.h"

uint16_t spi_data;

#ifdef SPI_MASTER     
/*-----------------Инициализация SPI1-----------------*/         
void init_SPI1 (void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    SPI_I2S_DeInit(SPI1);
    SPI_InitTypeDef spi1;
    SPI_StructInit(&spi1);
    spi1.SPI_Mode = SPI_Mode_Master;
    spi1.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi1.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    spi1.SPI_DataSize = SPI_DataSize_8b;
    spi1.SPI_CPOL = SPI_CPOL_Low;
    spi1.SPI_CPHA = SPI_CPHA_2Edge;
    spi1.SPI_NSS = SPI_NSS_Soft;
    spi1.SPI_FirstBit = SPI_FirstBit_MSB; 
    spi1.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &spi1);
                
    SPI_Cmd(SPI1,ENABLE);    
}
/*-------------------Отправка данных------------------*/
void SPI_send(uint8_t data) 
{
    while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1,data);
}

#endif

#ifdef SPI_SLAVE   
/*-----------------Инициализация SPI2-----------------*/                  
void init_SPI2 (void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    SPI_I2S_DeInit(SPI2);
    SPI_InitTypeDef spi2;
    SPI_StructInit(&spi2);
    spi2.SPI_Mode = SPI_Mode_Slave;
    spi2.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi2.SPI_DataSize = SPI_DataSize_8b;
    spi2.SPI_CPOL = SPI_CPOL_Low;
    spi2.SPI_CPHA = SPI_CPHA_2Edge;
    spi2.SPI_NSS = SPI_NSS_Soft;
    spi2.SPI_FirstBit = SPI_FirstBit_MSB;
    spi2.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &spi2);
 
    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);
    
    SPI_Cmd(SPI2, ENABLE);
    
    NVIC_EnableIRQ(SPI2_IRQn);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
}
/*-------------------Чтение данных-------------------*/ 
void SPI2_IRQHandler()
{
    spi_data = SPI_I2S_ReceiveData(SPI2);
}

#endif
