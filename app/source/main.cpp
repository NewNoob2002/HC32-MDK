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
#include "HardwareSerial.h"
#include "hc32_ll.h"
#include <Arduino.h>
#include <cm_backtrace.h>
#include "HardwareI2cSlave.h"
#include "MillisTaskManager.h"
MillisTaskManager task;
/**
 * @addtogroup HC32F460_DDL_Examples
 * @{
 */

/**
 * @addtogroup SPI_DMA
 * @{
 */

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | \
                           LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)

static void led_blink()
{
    GPIO_TogglePins(GPIO_PORT_B, GPIO_PIN_14);
    // Serial.printf("%d", ringBuffer.get_read_index());
}

static void serial_read()
{
    static uint8_t ch[128];
    static int index = 0;

    if (SlaveRxBuffer->count() > 0) {
        ch[index++] = Slave_Read();
    }

    if (index > 127) { // 如果收到换行符或者缓冲区已满，则处理并打印数据
        for (int i = 0; i < index; i++) {
            Serial.printf("0x%02X ", ch[i]);
        }
        Serial.println();
        index = 0;
    }
}
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
    clock_init();
    systick_init();
    Serial.begin(115200);
    pinMode(PB14, OUTPUT);
    Slave_Initialize();
    /* Peripheral registers write protected */
    LL_PERIPH_WP(EXAMPLE_PERIPH_WP);

    task.Register(led_blink, 100);
    task.Register(serial_read, 100);
    //    task.Register(dmaSend, 10);

    while (1) {
        task.Running(millis());
    }
}
