#ifndef __DELAY_H
#define __DELAY_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/* Exported types ------------------------------------------------------------*/
extern volatile uint32_t delay_value;
extern volatile uint32_t ticks_value;

/* Exported functions ------------------------------------------------------- */
void delay_init(void);
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
uint32_t systick_time(void);

#endif





