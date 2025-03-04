#include "usart.h"	  

int USAR_RxData;
uint8_t USAR_RxFlag;
uint8_t Receive_Flag;


uint8_t rx_buffer[BUFFER_SIZE];

//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout; 
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
    USART3->DR = (u8) ch;      
	return ch;
}
#endif 


void uart3_init(u32 Baudrate)
{
	//���ô��ڲ���
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//����GPIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//����GPIOB10
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//����GPIOB11
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = Baudrate;//���ò�����ΪBaudrate
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ��������-��ʹ������
	USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;//����Ϊ����+����ģʽ
	USART_InitStructure.USART_Parity = USART_Parity_No;//У��λ��No��У��(��)  Odd��У��  EvenżУ��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ��1λֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ���8λ(��Ϊ�˳�������У��λ)
	USART_Init(USART3, &USART_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel=USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority= 0;
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);//����RXNE��־λ��NVIC�����
	USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);
	
	USART_Cmd(USART3, ENABLE);//ʹ�ܴ���
	
	
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
/*----------------------����Ϊ����(Txd)�йغ���-----------------------------*/

void USAR_SendByte(uint8_t Byte)//��Tx���ŷ���һ���ֽ�����-(ֻ֧��16����������0x41='A')
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//�ȴ����ݴ������
}

void USAR_SendArray(uint8_t *Array, uint16_t Length)//��Tx���ŷ�����ǰ�������������� USAR_SendArray(Array,5)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		USAR_SendByte(Array[i]);
	}
}

void USAR_SendString(char *String)//��Tx���ŷ����ַ���
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		USAR_SendByte(String[i]);
	}
}

uint32_t USAR_Pow(uint32_t X, uint32_t Y)//��Tx���ŷ���һ������(����)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void USAR_SendNumber(uint32_t Number, uint8_t Length)//��Tx���ŷ���һ������
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		USAR_SendByte(Number / USAR_Pow(10, Length - i - 1) % 10 + '0');
	}
}	
/*----------------------����Ϊ����(Rxd)�йغ���-----------------------------*/

uint8_t USAR_GetRxFlag(void)//�õ��������ݵı�־λ
{
	if(USAR_RxFlag == 1)
	{
		USAR_RxFlag=0;
		return 1;
	}
	return 0;
}

uint8_t USAR_GetRxData(void)//�õ����յ������ݵ�ֵ
{
	return USAR_RxData;
}

void Usart3_Send(u8 *buf,u8 len)
{
	u8 t;
  	for(t=0;t<len;t++)		//ѭ����������
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
	memset(buf, 0, BUFFER_SIZE);  // ������������
}


