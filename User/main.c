#include "stm32f4xx.h"                  // Device header
#include "MTSP.h"

#define F_TIMER 84000000

void delay(int ms);
int main()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	  
	InitMaster(GPIOB, GPIO_Pin_6, GPIO_Pin_7,GPIO_PinSource6, GPIO_PinSource7,USART1_IRQn, USART1, 0xFF, 9600);
	//InitSlave(GPIOB, GPIO_Pin_6, GPIO_Pin_7,GPIO_PinSource6, GPIO_PinSource7,USART1_IRQn, USART1, 0xFF,0x01, 9600);
	while(1)
	{
		//byte message[MESSAGE_LENGTH] = {0x04, 0x03, 0x01};
		//SendRequest(message);
		//delay(100);
	}

}


void delay(int ms)
{
	TIM6->PSC = F_TIMER/(3000) - 1;
	TIM6->ARR = ms*3;
	TIM6->EGR = TIM_EGR_UG;
  TIM6->CR1 = TIM_CR1_CEN|TIM_CR1_OPM;
  while ((TIM6->CR1 & TIM_CR1_CEN)!=0);
}


// Обработчик прерывания
void USART1_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		parse(USART_ReceiveData(USART1));
	}
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}
