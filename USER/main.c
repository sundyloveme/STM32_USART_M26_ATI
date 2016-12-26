/**
 ******************************************************************************
 * @author  None
 * @brief   USART通信 以及GPRS(M26)通信
 */ 

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x_it.h"	 
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "misc.h"

#define POOLSIZE 256
#define GSM_PWR	   GPIO_Pin_12
#define GSM_POWER(x) (x ? GPIO_SetBits(GPIOA,GSM_PWR):GPIO_ResetBits(GPIOA,GSM_PWR))

void delay_init(void);
int  M26_Init(void);
void M26_SendStr(const char * str);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void); 
void USART_Configuration(void);
void delay_ms(uint32_t nms);

/* 接收M26响应的缓冲区 */
uint8_t *bufferM26;

/* 注意使用前后初始化 */
uint16_t i_BufferM26=0;

/* ATI响应结束标识 */
uint8_t received_ATI_OK=0;

/* 接收USART1字符串的缓冲区 */
char *buf;

/**
 * @brief 解析出字符串中的词汇
 * @retval 词汇的地址 注意释放
 * TODO
 */
 char *nextWord()
 {
     uint8_t temp[64]={0};
     for(int i=1;i<=64;i++)
     {
        

     }


 }

/**
 * @brief ATI响应解析器 <key,value>模式
 * @param 要解析的Key
 * @retval 找到返回Value 找不到返回空指针
 * @author Sundy
 */
char* ATI_wordExpress(const char *key)
{
    /* bufferM26的拷贝 */
    uint8_t* old_bufferM26 = bufferM26;

    /* 解析的原理是在母串中匹配Key,成功后返回之后的母串 */
    for(uint16_t i=0;i<=POOLSIZE;i++)
    {
        if(memcmp(key,old_bufferM26,strlen(key))==0)
        {
            old_bufferM26 += strlen(key);
            return old_bufferM26; 
        }
        else
        {
            old_bufferM26++;
        }
    }
    return NULL;
}

 /**
 * @brief 发送字符串到USART1上
 * @param  None
 * @retval None  
 * @author sundy
 */
void sendStrToUsart1(const char *str)
{
    while(*str)
    {
        int retry=0;
        USART_SendData(USART1,*str);
        str++;
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
        {
            retry++;
            if(retry>2000)
            {
                break;   
            }            
        }
    }
}


/**
 * @brief 接收来自USART1的字符串 注意尺寸256
 * @param   buf：存放接收信息的字符串的指针
 * @retval  1:接收成功 0:接收失败
 * @author sundy
 */
int receivedDataFromUSART1(char **buf)
{
    int retry=0;
    int bufIt=0;

    memset(*buf,0,sizeof(char)*256);

    for(int i=1;;i++)
    {
        retry=0;
        while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != SET)
        {
            /* 如果始终收不到信息那么就退出此函数 */
            retry++;
            if(retry>2000)
            {
                if(i==1)
                {
                    return 0;/* 如果第一次接收不到, 那么返回失败 */
                }
                return 1;
            }          
        }

        (*buf)[bufIt]=USART_ReceiveData(USART1);
        bufIt++;
    }
}

/**
 * @brief 入口    
 * @param  None
 * @retval None
 * @author Sundy
 */
int main(void)
{
	bufferM26=(uint8_t*)malloc(sizeof(uint8_t)*POOLSIZE);
	buf=(char*)malloc(sizeof(char)*64);
	
    /* 初始化bufferM26 */
	memset(bufferM26,0,sizeof(uint8_t)*POOLSIZE);
    
    RCC_Configuration();
    GPIO_Configuration();
    USART_Configuration();
    /* PB3是USART的电源控制 */
    GPIO_ResetBits(GPIOB,GPIO_Pin_3);
			
    delay_init() ;
    GSM_POWER(1);
    M26_Init();
	
    /* M26初始化的响应 */
	/* received_ATI_OK标志位起来说明ATI响应结束 */
	while(received_ATI_OK!=1)
	{
	}
    /* 响应输出成功后必须初始化 */
	/* 初始化bufferM26缓冲区, received_ATI_OK标志位, i_BufferM26下标 */
	received_ATI_OK=0;
    memset(bufferM26, 0, sizeof(uint8_t)*POOLSIZE);
    i_BufferM26=0;
    /* 屏幕上打印启动成功 */
	sendStrToUsart1("THE M26 START SUCCESSFULLY!\n");
	sendStrToUsart1("Please input ATI code...\n");
	
	/* 发送"AT" 并返回响应 */
	M26_SendStr("AT\r\n");
	while(received_ATI_OK!=1)
	{
	}
	sendStrToUsart1(bufferM26);
	sendStrToUsart1("===============================\n");
	/* 响应输出成功后必须初始化 */
	/* 初始化bufferM26缓冲区, received_ATI_OK标志位, i_BufferM26下标 */
	received_ATI_OK=0;
	memset(bufferM26, 0, sizeof(uint8_t)*POOLSIZE);
	i_BufferM26=0;
	
	/* 发送"ATI" 并返回响应 */
	M26_SendStr("ATI\r\n");
	while(received_ATI_OK!=1)
	{
	}
	sendStrToUsart1(bufferM26);
	sendStrToUsart1("===============================\n");
	/* 解析Revision */
    char *p_temp;
    if( (p_temp = ATI_wordExpress("Revision:")) != NULL)
    {
        for(uint8_t i=0;i<=POOLSIZE;i++)
        {
            if(p_temp[i]=='\n')
            {
                /* 区分Value与其他字符 */
                p_temp[i]=0;
                break;
            }
        }
    }
    sendStrToUsart1(p_temp);
    /* 响应输出成功后必须初始化 */
	/* 初始化bufferM26缓冲区, received_ATI_OK标志位, i_BufferM26下标 */
	received_ATI_OK=0;
	memset(bufferM26, 0, sizeof(uint8_t)*POOLSIZE);
	i_BufferM26=0;
	
    /* 发送"AT+CGMI" 并返回响应 */
	M26_SendStr("AT+CGMI\r\n");
	while(received_ATI_OK!=1)
	{
	}
	sendStrToUsart1(bufferM26);
	sendStrToUsart1("===============================\n");
	/* 解析Revision */
    //char *p_temp;
    p_temp=NULL;
    if( (p_temp = ATI_wordExpress("Revision:")) != NULL)
    {
        for(uint8_t i=0;i<=POOLSIZE;i++)
        {
            if(p_temp[i]=='\n')
            {
                /* 区分Value与其他字符 */
                p_temp[i]=0;
                break;
            }
        }
    }
    sendStrToUsart1(p_temp);
    /* 响应输出成功后必须初始化 */
	/* 初始化bufferM26缓冲区, received_ATI_OK标志位, i_BufferM26下标 */
	received_ATI_OK=0;
	memset(bufferM26, 0, sizeof(uint8_t)*POOLSIZE);
	i_BufferM26=0;
	
	/* 串口接收ATI CODE 返回ATI响应*/
	// while(1)
	// {
	// 	/* 必须初始化 */
	// 	memset(buf, 0, sizeof(char)*64);
		
	// 	/* USART1成功接收ATI CODE */
	// 	if(receivedDataFromUSART1(&buf)==1)
	// 	{
	// 		M26_SendStr(buf);
	// 		M26_SendStr("\r\n");
	// 		while(received_ATI_OK!=1)
	// 		{
	// 		}
	// 		sendStrToUsart1(bufferM26);
	// 		sendStrToUsart1("===============================\n");
	// 		/* 响应输出成功后必须初始化
	// 		bufferM26缓冲区, received_ATI_OK标志位, i_BufferM26下标 */
	// 		received_ATI_OK=0;
	// 		memset(bufferM26, 0, sizeof(uint8_t)*POOLSIZE);
	// 		i_BufferM26=0;
	// 	}
	// }
}

/**
 * @brief	时钟配置
 * @param   None
 * @retval  None
 */
void RCC_Configuration(void) 
{
    //SystemInit();//72m
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,   ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* USART1_TX -> PA9 , USART1_RX ->	PA10 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // pin2,3,4,9可能对应电源
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_9;  
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    // M26的电源的引脚
    GPIO_InitStructure.GPIO_Pin = GSM_PWR;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

void NVIC_Configuration(void) //优先级
{

    NVIC_InitTypeDef NVIC_InitStructure; 
    //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* Configure the USART1 synchronous paramters */
void USART_Configuration(void)
{




    USART_InitTypeDef  USART_InitStructure;

    USART_InitStructure.USART_BaudRate            = 9600;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;

    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件控制
    USART_InitStructure.USART_Mode                = USART_Mode_Rx|USART_Mode_Tx;

    USART_Init(USART1, &USART_InitStructure);   
    USART_Cmd(USART1, ENABLE);      //串口1使能 

    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); //打开接收中断	

    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);

}
