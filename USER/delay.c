/**
 ******************************************************************************
 * @file    delay.c
 * @author  
 * @version 0.2
 * @date    07/01/2015
 * @brief   Delay functions.
 *          Delay n us or n ms.
 ******************************************************************************
 */ 

#include "delay.h"

volatile uint32_t ticks_value = 0; // 毫秒计数，仅用于计数，不用于delay。
volatile uint32_t delay_value = 0; // delay 时间，以需要完成多少次 sys tick 来计算

/**
 * @brief   初始化延迟函数  （SYSTICK的时钟固定为HCLK时钟的1/8）
 * @param   None
 * @retval  None
 * @author  yinnnian
 * @date    2015-11-9
 * @author  pengtian
 * @date    2016-3-14
 * @version 2.0
 */
void delay_init(void)
{
    /* 初始化计数值 */
    delay_value = 0;
    ticks_value = 0;
    /* 设置 SysTick 为 1ms 一次 */
    SysTick_Config(SystemCoreClock / 1000);
    /* 在 NVIC 上启用 SysTick 中断, 该中断为 Cortex-M System Interrupts */
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
}

/**
 * @brief   延时 * ms
 *          等待时间内进入 WFI
 * @param   nms：要延时的ms数
 * @retval  None
 * @author  pengtian
 * @date    2016-01-11
 * @Modified
 */
void delay_ms(uint32_t nms)
{
    delay_value = nms;

    while (delay_value)
    {
        __WFI();
    }
}

/**
 * @brief   获取当前 SysTick 计数值，默认单位为 ms
 *          连续开机 2 ^ 32 ms （49天）之后，计数值会溢出
 * @param   NA
 * @retval  SysTick 计数值
 * @author  pengtian
 * @date    2016-03-08
 * @Modified
 */
uint32_t systick_time(void)
{
    return ticks_value;
}

