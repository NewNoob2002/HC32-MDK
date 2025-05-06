#pragma once

#include <rtthread.h>
#include <settings.h>
#include "debug.h"
//#include "Charger.h"
//#include "Fuelgauge.h"
#include "Key.h"
#include "adc.h"
#include "Led.h"
#include "Task.h"
#include "powerControl.h"
#include "System.h"
#include "mcu_define.h"

extern uint16_t batteryLevelPercent;
extern float batteryTempC;
extern float batteryChargingPercentPerHour;
extern float batteryVoltage;
extern uint8_t POWER_OFF_FLAG;
