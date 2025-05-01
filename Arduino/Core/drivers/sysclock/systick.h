#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void systick_init();
uint32_t millis();
uint32_t micros();

#ifdef __cplusplus
}
#endif