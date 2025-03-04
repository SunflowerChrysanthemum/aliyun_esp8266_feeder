#include "stm32f10x.h"                  // Device header
#include "Delay.H"
#include "usart.h"
#include "ESP8266.h"
#include "OLED.h"
#include "string.h"
#include "HX711.h"
#include "timer.h"

uint8_t total_weight;
uint8_t tub_weight;
float weight_gross;

int main()
{
	uart3_init(115200);
	OLED_Init();
	HX711_Init();
	HX711_Maoqi();
	Delay_ms(800);
	HX711_Maoqi();
	
	timer2_Init();
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
//		Usart3_Send(rx_buffer, data_length);
		
		if(start_flag)
		{
			checkOK(rx_buffer);
		}
		if(data_flag)
		{
			Btn_Clicked();
//			printf("numb1=%c, numb2=%c, numb3=%c\r\n", rx_buffer[73], rx_buffer[74], rx_buffer[77]);
//			OLED_ShowChar(3, 1, rx_buffer[73]);
//			OLED_ShowChar(3, 2, rx_buffer[74]);
//			OLED_ShowChar(3, 3, rx_buffer[77]);
		}
		
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);
		DMA_Cmd(DMA1_Channel3, DISABLE );  //关闭USART1 TX DMA1所指示的通道    
		DMA_SetCurrDataCounter(DMA1_Channel3, BUFFER_SIZE);//DMA通道的DMA缓存的大小
		DMA_Cmd(DMA1_Channel3, ENABLE);  //打开USART1 TX DMA1所指示的通道 
	}
}


/* 每100ms进行一次中断 10秒发一次重量信息*/
void TIM2_IRQHandler(void)
{
	static uint8_t cnt = 0;
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		if(data_flag)
		{
			cnt++;
			if(cnt >= 100)
			{
				ESP8266_TotalWeight(weight_gross/10);
				cnt = 0;
				if(Auto_Feed_Global)
				{
					OLED_ShowNum(4, 1, 1, 1);
				}else{
					OLED_ShowNum(4, 1, 0, 1);
				}
			}
			weight_gross = Get_Weight();
			OLED_ShowNum(3, 1, weight_gross, 4);
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

