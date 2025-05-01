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

// 函数前向声明
static void analyzeI2cData(uint8_t* data, uint16_t length);

static void led_blink()
{
    GPIO_TogglePins(GPIO_PORT_B, GPIO_PIN_14);
    // Serial.printf("%d", ringBuffer.get_read_index());
}

static void serial_read()
{
    static uint8_t buffer[128];  // 存储数据的缓冲区
    static uint16_t bufferIndex = 0;  // 缓冲区当前位置
    static uint32_t lastReceiveTime = 0;  // 上次接收数据的时间
    const uint32_t timeoutMs = 50;  // 数据包超时时间(ms)
    
    // 1. 从环形缓冲区读取数据到本地buffer
    uint32_t availableBytes = SlaveRxBuffer->count();
    if (availableBytes > 0) {
        lastReceiveTime = millis();  // 更新接收时间
        
        // 读取数据，避免缓冲区溢出
        while (availableBytes > 0 && bufferIndex < sizeof(buffer)) {
            buffer[bufferIndex++] = Slave_Read();
            availableBytes--;
        }
    }
    
    // 2. 检查是否应该处理已接收的数据 (满足以下任一条件)
    // - 缓冲区已满
    // - 自上次接收数据起已超时(一个数据包的传输完成)
    bool shouldProcess = false;
    
    if (bufferIndex >= sizeof(buffer)) {
        shouldProcess = true;  // 缓冲区已满
    } else if (bufferIndex > 0 && (millis() - lastReceiveTime) > timeoutMs) {
        shouldProcess = true;  // 接收超时，认为一个数据包接收完成
    }
    
    // 3. 处理接收到的数据
    if (shouldProcess) {
        // 打印接收到的数据
        Serial.printf("I2C Received %d bytes: ", bufferIndex);
        for (uint16_t i = 0; i < bufferIndex; i++) {
            Serial.printf("0x%02X ", buffer[i]);
        }
        Serial.println();
        
        // 在这里可以添加更多数据分析和处理逻辑
        // 例如：分析数据包结构，解析命令，提取有效载荷等
        analyzeI2cData(buffer, bufferIndex);
        
        // 重置缓冲区索引，准备接收下一个数据包
        bufferIndex = 0;
    }
}

// 分析和处理I2C接收到的数据
static void analyzeI2cData(uint8_t* data, uint16_t length)
{
    // 检查数据包是否有效
    if (length < 1) {
        return;  // 数据包太短
    }
    
    // 假设数据包格式: [命令字节][数据...]
    uint8_t command = data[0];
    
    // 根据命令类型处理数据
    switch (command) {
        case 0x01:  // 示例：读取命令
            Serial.println("Read command received");
            // 处理读取命令
            break;
            
        case 0x02:  // 示例：写入命令
            if (length >= 3) {  // 确保有足够的数据
                uint8_t address = data[1];
                uint8_t value = data[2];
                Serial.printf("Write command: address=0x%02X, value=0x%02X\n", address, value);
                // 处理写入命令
            }
            break;
            
        case 0x03:  // 示例：状态查询命令
            Serial.println("Status query command received");
            // 处理状态查询
            break;
            
        default:
            Serial.printf("Unknown command: 0x%02X\n", command);
            break;
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
