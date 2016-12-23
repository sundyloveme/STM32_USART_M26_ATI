/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported macro ------------------------------------------------------------*/
#define GPS_FAIL_THRES      2       // GPS 保持供电，直到 2 次唤醒都无法成功定位
#define GPS_WAIT_TIME       1800    // 3 minutes, unit is 100ms
#define RTC_WAIT_TIME       299     // 5 minutes 300-1 等待rtc sync
// #define GPS_WAIT_TIME      50      // 5 seconds
// #define RTC_WAIT_TIME      59      // 60 seconds

#define GPRS_FRAME_DATA_NUM  10

#define IC_SIZE             10
#define SN_SIZE             10
#define IC_ADDR             0       // ic卡号存放的地址
#define SN_ADDR             15      // sn号存放的地址



#define IIC_RETRY_TIMES     200
#define IC_RETRY_TIMES      400000
#define SYS_VERSION         20160218


/* 电量 电压曲线分成三段 */
#define BATTERY_FULL        1.585   // 满电时AD电压
#define BATTERY_CRITICAL_H  1.373   // 100-10的临界电压
#define BATTERY_CRITICAL_M  1.31    // 10-2的临界电压
#define BATTERY_CRITICAL_L  1.29    // 2-1分段临界电压
#define BATTERY_EMPTY       1.27    // 关断时AD电压
#define BATTERY_PER_H       100     // 分段后前一段电压占98%
#define BATTERY_PER_M       10      // 分段后前一段电压占98%
#define BATTERY_PER_L       2       // 分段后后一段电压占2%

// #define SN_SAFETY                   // 注释掉时可以改sn
// #define FAKE_SENSOR                 // 去掉注释后发送假数据到服务器
// #define TEST_SERVER                 // 去掉注释发送数据到测试服务器

/* Exported types ------------------------------------------------------------*/

extern uint8_t AlmIndex;
extern uint8_t sn_valid;
extern uint8_t ReadBuffer_ic[15];  // 是eeprom的缓冲区
extern uint8_t ReadBuffer_sn[25];  // 是eeprom的缓冲区

/**
  * @brief  状态机状态
  */
enum Status
{
    IDLE,
    GET,
    PROCESS,
    SEND,
    STOP
};

/**
  * @brief  系统参数，控制系统可变更设置项
  */
struct Sys_Param
{
    uint8_t dbg_on;          // Debug message ON, 打印 debug 信息
    uint16_t rtc_wait_time;  // RTC 唤醒时间，单位秒
    uint16_t gps_wait_time;  // GPS 等待搜星时间，单位秒
    uint16_t gprs_wait_time; // GPRS
    uint32_t version;        // code version
};

extern struct Sys_Param sys_param;






/* Private function prototypes -----------------------------------------------*/

void RCC_Configuration(void);
int SN_Write(char* data);

#endif
