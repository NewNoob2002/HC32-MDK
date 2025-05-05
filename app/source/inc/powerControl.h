#pragma once

#include <stdint.h>
// #include "settings.h"
//macros
#define ADC_PIN_CH0  ADC_CH0   // 对应 PA0

#define MCU_ON_OFF_PIN     PB3     

#define WATCHDOG_FEED_PIN PA6

#define ADC_TIMEOUT_VAL 1000U
//变量定义
extern unsigned int PowerKeyTrigger;
extern unsigned int PowerOffCount;
extern unsigned int PowerOffFlag;
//函数声明
void adcInit();
void AdcPolling(); // 检测关机按键电压

void PowerControlInit();

void Power_Control_Pin_Init();
void Power_Control_Pin_Switch(uint8_t off_on); //0:关机 1:开机

void WatchdogFeedPinInit();
void WatchdogFeed();
