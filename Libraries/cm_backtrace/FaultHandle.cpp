#include <hc32_ll.h>
//#include "Version..h"
#include <Arduino.h>
#include "cm_backtrace.h"

static void Delay(uint32_t ms)
{
    volatile uint32_t i = SystemCoreClock / 1000 * ms / 5;
    while(i--);
}

static void Reboot()
{
    Delay(1000);
    NVIC_SystemReset();
}

void cmb_printf(const char *__restrict __format, ...)
{
    char printf_buff[256];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);
    
//    Serial.print(printf_buff);
}