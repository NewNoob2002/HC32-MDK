#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief update the SYSTEM_CLOCK_FREQUENCIES variable
 */
void usart_init(uint32_t baud);

void rt_hw_console_output(const char *str);
	
#ifdef __cplusplus
}
#endif