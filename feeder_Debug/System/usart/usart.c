#include "usart.h"	  

int USAR_RxData;
uint8_t USAR_RxFlag;
uint8_t Receive_Flag;


uint8_t rx_buffer[BUFFER_SIZE];

//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout; 
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 


void uart3_init(u32 Baudrate)
{
	//配置串口部分
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//开启GPIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//开启GPIOB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//开启GPIOB11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = Baudrate;//设置波特率为Baudrate
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制-不使用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;//设置为发送+接收模式
	USART_InitStructure.USART_Parity = USART_Parity_No;//校验位：No无校验(√)  Odd奇校验  Even偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位：1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长：8位(因为此程序中无校验位)
	USART_Init(USART3, &USART_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority= 0;
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//开启RXNE标志位到NVIC的输出
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	
	USART_Cmd(USART3, ENABLE);//使能串口
	
	
	DMA_DeInit(DMA1_Channel3);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART3->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)rx_buffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel3, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);
}
/*----------------------以下为发送(Txd)有关函数-----------------------------*/

void USAR_SendByte(uint8_t Byte)//从Tx引脚发送一个字节数据-(只支持16进制数据如0x41='A')
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//等待数据传输结束
}

void USAR_SendArray(uint8_t *Array, uint16_t Length)//从Tx引脚发送提前定义的数组的数据 USAR_SendArray(Array,5)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		USAR_SendByte(Array[i]);
	}
}

void USAR_SendString(char *String)//从Tx引脚发送字符串
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		USAR_SendByte(String[i]);
	}
}

uint32_t USAR_Pow(uint32_t X, uint32_t Y)//从Tx引脚发送一个数字(辅助)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void USAR_SendNumber(uint32_t Number, uint8_t Length)//从Tx引脚发送一个数字
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		USAR_SendByte(Number / USAR_Pow(10, Length - i - 1) % 10 + '0');
	}
}	
/*----------------------以下为接收(Rxd)有关函数-----------------------------*/

uint8_t USAR_GetRxFlag(void)//得到接收数据的标志位
{
	if(USAR_RxFlag == 1)
	{
		USAR_RxFlag=0;
		return 1;
	}
	return 0;
}

uint8_t USAR_GetRxData(void)//得到接收到的数据的值
{
	return USAR_RxData;
}

void Usart3_Send(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
}

void OLED_Usart3(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)
	{		   
		OLED_ShowChar(1, t+1, buf[t]);
	}
}


void judge(u8 *buf, u8 len)
{
    u8 t;
    int foundOK = 0;

    for (t = 0; t < len - 1; t++)
    {   
        if (buf[t] == 'O' && buf[t+1] == 'K') 
        {
            OLED_ShowString(1, 1, "OK");
            foundOK = 1;
            break;
        }
    }

    if (!foundOK) {
        OLED_ShowString(1, 1, "ERR");
    }
}


void checkOK(u8 *buf)
{
    if (strstr((const char *)buf, "OK") != NULL) 
	{
		ok_flag = 1;
        OLED_ShowString(1, 1, "OK");
		OLED_ShowNum(1, 4, start_flag, 1);
    }
	else 
	{
        OLED_ShowString(2, 1, "ERR");
		OLED_ShowNum(2, 4, start_flag, 1);
    }
	memset(buf, 0, BUFFER_SIZE);  // 将缓冲区清零
}


