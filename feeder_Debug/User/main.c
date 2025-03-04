#include "stm32f10x.h"                  // Device header
#include "Delay.H"
#include "usart.h"
#include "ESP8266.h"
#include "OLED.h"
#include "string.h"

uint8_t total_weight;
uint8_t tub_weight;

int main()
{
	uart3_init(115200);
	ESP8266_Init();
	while(1)
	{
		
	}
}

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData(USART3);

//		uint16_t data_length = BUFFER_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);
		
//		Usart3_Send(rx_buffer, data_length);
//		OLED_Usart3(rx_buffer, data_length);
		if(start_flag)
		{
			checkOK(rx_buffer);
		}
			
		
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);
		DMA_Cmd(DMA1_Channel3, DISABLE );  //�ر�USART1 TX DMA1��ָʾ��ͨ��    
		DMA_SetCurrDataCounter(DMA1_Channel3, BUFFER_SIZE);//DMAͨ����DMA����Ĵ�С
		DMA_Cmd(DMA1_Channel3, ENABLE);  //��USART1 TX DMA1��ָʾ��ͨ�� 
	}
}

