#include "spi.h"
#include "i2c.h"
#include "pwm.h" 

int main(void)
{
    #ifdef SPI_MASTER
    init_SPI1();
    #endif
    
    #ifdef SPI_SLAVE
    init_SPI2();
    #endif
    
    #ifdef I2C_MASTER
    I2C1_init();
    #endif
    
    #ifdef I2C_SLAVE
    I2C2_init();
    #endif
    
	PWM_init();
    
    while(1)
    {
        //Конкретных задач как таковых не было, так что здесь может быть что угодно
    }
}


// классический ассерт для STM32
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t * file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
     
    (void)file;
    (void)line;

    __disable_irq();
    while(1)
    {
        // это ассемблерная инструкция "отладчик, стой тут"
        // если вы попали сюда, значит вы ошиблись в параметрах. Смотрите в call stack
        __BKPT(0xAB);
    }
}
#endif
