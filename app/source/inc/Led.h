#pragma once

#include <stdint.h>
#if 1
#define CHG_LED_PIN PH2
#define FunctionKey_PIN PA15
#else
#define CHG_LED_PIN PB14
#define FunctionKey_PIN PB15
#endif
#define POWER_LED_PIN PC13  
#define DATA_LED_PIN PB7
#define GNSS_STATE_LED_PIN PB6
#define FunctionKey_LED_PIN PB5

void ledInit();
void chargeLedSwitch(uint8_t newState);
void chargeLedBlink(int rate);

void powerLedSwitch(uint8_t newState);
void powerLedBlink(int rate);

void dataLedSwitch(uint8_t newState);
void dataLedBlink(int rate);

void gnssLedSwitch(uint8_t newState);
void gnssLedBlink(int rate);

void functionKeyLedSwitch(uint8_t newState);
void functionKeyLedBlink(int rate);

void ChargerLedUpdate();
void PowerLedUpdate();
void DataLedUpdate();
void GnssLedUpdate();
void FunctionKeyLedUpdate();
