#include <rtthread.h>
#include <Arduino.h>

#ifndef LOG_DEBUG_H
#define LOG_DEBUG_H

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

#define LOG_COLOR_DEBUG "\033[34m"
#define LOG_COLOR_INFO "\033[32m"
#define LOG_COLOR_WARN "\033[33m"
#define LOG_COLOR_ERROR "\033[31m"
#define LOG_COLOR_END "\033[0m"

#define LOG_LEVEL LOG_LEVEL_DEBUG

#define LOG_DEBUG(fmt, args...) if(LOG_LEVEL_DEBUG >= LOG_LEVEL) Serial.printf(LOG_COLOR_DEBUG "[DEBUG] " fmt LOG_COLOR_END "\n", ##args)
#define LOG_INFO(fmt, args...) if(LOG_LEVEL_INFO >= LOG_LEVEL) Serial.printf(LOG_COLOR_INFO "[INFO] " fmt LOG_COLOR_END "\n", ##args)
#define LOG_WARN(fmt, args...) if(LOG_LEVEL_WARN >= LOG_LEVEL) Serial.printf(LOG_COLOR_WARN "[WARN] " fmt LOG_COLOR_END "\n", ##args)
#define LOG_ERROR(fmt, args...) if(LOG_LEVEL_ERROR >= LOG_LEVEL) Serial.printf(LOG_COLOR_ERROR "[ERROR] " fmt LOG_COLOR_END "\n", ##args)

#endif