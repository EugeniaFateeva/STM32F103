#include "pwm.h"

//Изменяя frequence и value получаем разные меандры
volatile int16_t frequence = 10;
volatile int16_t value = 500;

/*-------------Обработчик прерываний ШИМ------------*/
void TIM3_IRQHandler(void)
{
    TIM_SetCompare3(TIM3, value);
    TIM3->PSC = (72000/frequence)-1;
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
}

/*----------------Инициализация ШИМ----------------*/
void PWM_init (void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_8;
	gpio.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &gpio);
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseInitTypeDef timer_base;
	TIM_TimeBaseStructInit(&timer_base);
	timer_base.TIM_Prescaler = 1;
	timer_base.TIM_Period = 1000-1;
    timer_base.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &timer_base);
	
	TIM_OCInitTypeDef timer_oc;
	TIM_OCStructInit(&timer_oc);
	timer_oc.TIM_OCMode=TIM_OCMode_PWM1;  
    timer_oc.TIM_Pulse = 500;
	timer_oc.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OC3Init (TIM3, &timer_oc);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	NVIC_EnableIRQ(TIM3_IRQn);
}
