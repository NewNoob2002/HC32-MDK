#include <hc32_ll.h>
#include <Arduino.h>
#include <cm_backtrace.h>
static void Delay(uint32_t ms)
{
    volatile uint32_t i = F_CPU / 1000 * ms / 5;
    while (i--);
}

static void Reboot()
{
    Delay(1000);
    NVIC_SystemReset();
}

extern "C" {
void cmb_printf(const char *__restrict __format, ...)
{
    char printf_buff[256];

    va_list args;
    va_start(args, __format);
    int ret_status = vsnprintf(printf_buff, sizeof(printf_buff), __format, args);
    va_end(args);

    //    Serial.print(printf_buff);
}

void vApplicationHardFaultHook();
// 使用正确的函数声明，与cm_backtrace.h一致
// void cm_backtrace_fault(uint32_t lr, uint32_t* sp);

void vApplicationHardFaultHook()
{
    // HAL::Display_DumpCrashInfo("FXXK HardFault!");
    Reboot();
}

void HardFault_Handler(void) __attribute__((naked));
void HardFault_Handler(void)
{
    __asm volatile(
        "mov r0, lr \n"
        "mov r1, sp \n"
        "bl cm_backtrace_fault \n"
        "bl vApplicationHardFaultHook \n"
        "fault_loop: \n"
        "b fault_loop \n");
}

}