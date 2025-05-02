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
	
#ifdef __cplusplus
}
#endif