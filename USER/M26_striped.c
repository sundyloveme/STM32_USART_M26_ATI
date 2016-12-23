/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "string.h"
#include "delay.h"
#include "main.h"

#define GSM_STUTAS GPIO_Pin_8 
#define GSM_PWRKEY GPIO_Pin_9 
#define GSM_PWR	   GPIO_Pin_12
#define GSM_POWER(x) x? GPIO_SetBits(GPIOA,GSM_PWR):GPIO_ResetBits(GPIOA,GSM_PWR)

void sendStrToUsart1(const char *str);
/*M26 GPRS模快

  M26和主MCU通过串口通信，有四线连接主MCU
  使用的串口为UART2  PA2 PA3
  */
	

//// =======
//void delay_ms(uint32_t nms)
//{
//    int i;
//    for(i=0;i<(nms*5500);i++)
//    {
//    }
//}
  
/**
 * @brief   GPRS使能
 * @param   None
 * @retval  None
 * @author  yinnnian
 * @date    2015-11-9
 * @Modified
 */
void M26_GPIO_Enable(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);

    /* 主MCU的TX  gprs的rx */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    /* 主MCU的RX  gprs的Tx */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  	
    GPIO_Init(GPIOA,&GPIO_InitStructure);  

    /* GSM_STUTAS */
    GPIO_InitStructure.GPIO_Pin   = GSM_STUTAS;
    /* 上拉输ru */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GSM_PWRKEY;// GSM_PWRKEY  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 

 /*    GPIO_InitStructure.GPIO_Pin = GSM_PWR;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	 */

}

/**
 * @brief   GPRS失能
 * @param   None
 * @retval  None
 * @author  yinnnian
 * @date    2015-11-9
 * @Modified
 */
//void M26_GPIO_Disable(void)
//{
//    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);

//    /* 主MCU的TX  gprs的rx */
//    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_SAVE;
//    GPIO_Init(GPIOA,&GPIO_InitStructure);

//    /* 主MCU的RX  gprs的Tx */
//    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_SAVE;
//    GPIO_Init(GPIOA,&GPIO_InitStructure);  

//    /* GSM_STUTAS */
//    GPIO_InitStructure.GPIO_Pin   = GSM_STUTAS;
//    GPIO_InitStructure.GPIO_Mode  = GPIO_MODE_SAVE;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//}

/**
 * @brief   GPRS串口配置
 * @param   None
 * @retval  None
 * @author  yinnnian
 * @date    2015-11-9
 * @Modified
 */
void M26_UART_Cfg(void)
{
    uint32_t retry=0;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

   	// GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

    USART_InitStructure.USART_BaudRate              = 9600;
    USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits              = USART_StopBits_1;
    USART_InitStructure.USART_Parity                = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2,&USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    /* 默认为接收模式 */
    USART_Cmd(USART2,ENABLE);

    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {
        retry++;
        if(retry>2000)
        {
            break;
        }
    }        
}

/**
 * @brief   GPRS串口中断优先级配置
 * @param   None
 * @retval  None
 * @author  yinnnian
 * @date    2015-11-9
 * @Modified
 */
void NVIC_UART2_M26_Configuration(void) 
{ 
    NVIC_InitTypeDef NVIC_InitStructure;
    /* NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  */
    NVIC_InitStructure.NVIC_IRQChannel                      = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = 1; 
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE; 
    NVIC_Init(&NVIC_InitStructure); 
}

/**
 * @brief   GPRS发送字符串
 * @param   str：AT指令
 * @retval  None
 * @author  yinnnian
 * @date    2015-11-9
 * @Modified
 */
void M26_SendStr(const char * str)
{
    uint32_t retry=0;	  
    while(*str)  
    {   
        retry=0;        
        USART_SendData(USART2, *str);  		
	    str++;
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
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
 * @brief   GPRS发送1字节数据
 * @param   Data：待发送的字节
 * @retval  1：失败 0：成功
 * @author  yinnnian
 * @date    2015-11-9
 * @Modified
 */
uint8_t M26_SendByte(uint16_t Data)
{
    uint32_t retry=0;
    USART_SendData(USART2, Data);                            
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {
        retry++;
        if(retry>2000)
        {
            return 1;  
        }            
    }
    return 0;
}



/**
 * @brief  M26 初始化，通过 pwrkey 电平控制完成开机，并通过 AT 指令完成串口同步
 *         尝试开机三次，失败则退出
 * @param  None
 * @retval fail：0  success: 1
 */
int M26_Init(void)
{
    uint32_t retry   = 0; // M26 开机重试次数
    uint32_t timeout = 0; // 等待关机的时间计数，单位 100 ms
    int init_cmpl    = 0; // 初始化完成

    M26_GPIO_Enable();
    M26_UART_Cfg();
    NVIC_UART2_M26_Configuration();

    //dbg_printf("[M26] start up\r\n");
		

    do
    {
        retry++;
        /* 当前 M26 已经关机，执行开机流程 */
        if(GPIO_ReadInputDataBit(GPIOB, GSM_STUTAS)==0)//关机状态
        {
            /* 拉高 300ms */
            GPIO_ResetBits(GPIOB, GSM_PWRKEY);
            delay_ms(300);
						
            /* 拉低 5s，M26 在开机后4-5s才能响应 AT。直到正常响应 AT 指令，再拉高 */
            GPIO_SetBits(GPIOB, GSM_PWRKEY);
            delay_ms(5000);
						
            // TODO 发送 AT 自适应
            M26_SendStr("ATI\r\n");
            //dbg_printf("\n");
        }
    } while((init_cmpl == 0) && retry < 3); // AT 自适应不成功

    /* 拉高 */
    GPIO_ResetBits(GPIOB, GSM_PWRKEY);

    return init_cmpl;
}
