/**
 *******************************************************************************
 * @file  spi/spi_dma/source/main.c
 * @brief Main program SPI tx/rx dma for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
   2023-09-30       CDT             Modify the IO properties of SPI
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <hc32_ll.h>
#include <Arduino.h>
#include <Task.h>
#include <settings.h>
#include <global.h>
#include <HardwareI2cSlave.h>
// #include "MillisTaskManager.h"
// MillisTaskManager task;
/**
 * @addtogroup HC32F460_DDL_Examples
 * @{
 */

/**
 * @addtogroup SPI_DMA
 * @{
 */
uint8_t chargeStatus                = notCharge;
float batteryLevelPercent           = 50; // SOC measured from fuel gauge, in %. Used in multiple places (display, serial debug, log)
float batteryVoltage                = 0.0;
float batteryChargingPercentPerHour = 0.0;
float batteryTempC                  = 0.0;
present_device present_devices;
online_device online_devices;

// system device
unsigned short reg_value        = 0;
static uint32_t lastPowerOnTime = 0;
SystemParameter DisplayPannelParameter;
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
static struct rt_thread i2cSlave_thread;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t i2cSlave_thread_stack[1024];
static rt_uint8_t i2cSlave_thread_priority = 6;

static struct rt_thread led_thread;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_thread_stack[1024];
static rt_uint8_t led_thread_priority = 6;

static struct rt_thread message_thread;
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t message_thread_stack[2048];
static rt_uint8_t message_thread_priority = 7;
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | \
                           LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)


/**
 * @brief  Main function of SPI tx/rx dma project
 * @param  None
 * @retval int32_t return value, if needed
 */
int main(void)
{
    /* Peripheral registers write unprotected */
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
    /* Configure BSP */
    Serial.begin(115200);
	
		//start Init device
	  unsigned short reg_value = *((unsigned short *)0x400540C0UL);
    if (reg_value & 0x0100U) {
        LOG_INFO("Software reset");
//        Power_Control_Pin_Switch(1);
    } else if (reg_value & 0x0002U) {
        LOG_INFO("EWDT or Hardware reset");
    } else if (reg_value & 0x2000U) {
        LOG_ERROR("XTAL error");
    }
    pinMode(PB14, OUTPUT);
    I2C_Slave.begin();

    /* Peripheral registers write protected */
    LL_PERIPH_WP(EXAMPLE_PERIPH_WP);

    // rt_thread_init(&i2cSlave_thread,
    //                "i2cSlave",
    //                i2c_slave_task,
    //                RT_NULL,
    //                &i2cSlave_thread_stack,
    //                sizeof(i2cSlave_thread_stack),
    //                i2cSlave_thread_priority,
    //                100);
    rt_thread_init(&led_thread,
                   "led",
                   led_task,
                   RT_NULL,
                   &led_thread_stack,
                   sizeof(led_thread_stack),
                   led_thread_priority,
                   100);
    rt_thread_init(&message_thread,
                   "led",
                   btReadTask,
                   RT_NULL,
                   &message_thread_stack,
                   sizeof(message_thread_stack),
                   message_thread_priority,
                   1000);             
    rt_thread_startup(&message_thread);
    rt_thread_startup(&led_thread);
}
