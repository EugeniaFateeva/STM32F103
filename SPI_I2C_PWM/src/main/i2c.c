#include "i2c.h"

#ifdef I2C_MASTER
/*--------------Инициализация I2C-master---------------*/
void I2C1_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef  gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &gpio);
 
    I2C_InitTypeDef  i2c;
    I2C_StructInit(&i2c);
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = 0x00;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c.I2C_ClockSpeed = 100000;
 
    I2C_Init(I2C1, &i2c);
    
    I2C_Cmd(I2C1, ENABLE);
}

/*--------------------Включение I2C--------------------*/
void I2C_start(void)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
    I2C_GenerateSTART(I2C1, ENABLE);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
}

/*--------------------Остановка I2C--------------------*/
void I2C_stop(void)
{
    I2C_GenerateSTOP(I2C1, ENABLE);
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
}

/*--------------Выбор ведомого устройства--------------*/
void I2C_slave_selection(uint8_t slaveAddress, uint8_t direction)
{
    I2C_Send7bitAddress(I2C1, slaveAddress << 1, direction);
 
    if (direction == I2C_Direction_Transmitter)
    {
        while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    }
    else if (direction == I2C_Direction_Receiver)
    { 
        while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    }
}

/*-------------------Запись данных---------------------*/
void I2C_write(uint8_t address, uint8_t data)
{
    I2C_start();
    I2C_slave_selection(address, I2C_Direction_Transmitter);
    I2C_SendData(I2C1, data);
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_stop();
}

/*-------------------Чтение данных---------------------*/
uint8_t i2c_read(uint8_t address)
{
    I2C_start();
    I2C_slave_selection(address, I2C_Direction_Receiver);
    while( !I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) );
    uint8_t data;
    data = I2C_ReceiveData(I2C1);
    I2C_stop();
    return data; 
}

#endif

#ifdef I2C_SLAVE

uint8_t tx_byte;
uint8_t rx_byte;

/*--------------Инициализация I2C-slave----------------*/
void I2C2_init(void) 
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitTypeDef  gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &gpio);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
    NVIC_Init(&NVIC_InitStructure);

    I2C_InitTypeDef  i2c;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c.I2C_ClockSpeed = 100000;

    I2C_Init(I2C2, &i2c);
   
    I2C_ITConfig(I2C2, I2C_IT_EVT, ENABLE);
    I2C_ITConfig(I2C2, I2C_IT_BUF, ENABLE);
    I2C_ITConfig(I2C2, I2C_IT_ERR, ENABLE); 
    
    I2C_Cmd(I2C2, ENABLE);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
}


void I2C2_ER_IRQHandler(void)
{
    if (I2C_GetITStatus(I2C2, I2C_IT_AF)) 
    {
        I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
    }
}

void I2C2_EV_IRQHandler(void) 
{
  uint32_t event;
  event=I2C_GetLastEvent(I2C2);

  if(event==I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED) 
  {
    I2C_ReadRegister(I2C2,I2C_Register_SR1);
    I2C_ReadRegister(I2C2,I2C_Register_SR2);
  }
  else if(event==I2C_EVENT_SLAVE_BYTE_RECEIVED) 
  {
    rx_byte=I2C_ReceiveData(I2C2);
  }
  else if(event == I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED) 
  {
    I2C_ReadRegister(I2C2,I2C_Register_SR1);
    I2C_ReadRegister(I2C2,I2C_Register_SR2);
    I2C_SendData(I2C2, tx_byte);
  }
  else if(event == I2C_EVENT_SLAVE_BYTE_TRANSMITTED) 
  {
    I2C_SendData(I2C2, tx_byte);
  }
  else if(event==I2C_EVENT_SLAVE_STOP_DETECTED) 
  {
	I2C_GetFlagStatus(I2C2, I2C_FLAG_STOPF);
    I2C_Cmd(I2C2, ENABLE); 
  }
}

#endif
