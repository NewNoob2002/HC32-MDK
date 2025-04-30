#include <hc32_ll.h>
//#include <addon_usart.h>
#include "usart.h"
//#include "core_hooks.h"
#include <core_debug.h>
//#include "yield.h"
#include <../irqn/irqn.h>
#include <../sysclock/sysclock.h>
#include <stdarg.h>
#include <stdio.h>


void cmb_printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // 将字符串发送到串口
    // 根据您的系统选择一种方式:
    
    // 方式1: 直接使用printf (如果重定向到串口)
    printf("%s", buffer);
    
    // 方式2: 如果有串口发送函数，则使用该函数
    // 例如: UART_SendString(UART_UNIT, buffer);
    
    // 方式3: 如果使用SEGGER RTT
    // SEGGER_RTT_WriteString(0, buffer);
}