#pragma once

#if 0
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
void chargeLedOn();
void chargeLedOff();
void chargeLedBlink(int rate);

void powerLedOn();
void powerLedOff();
void powerLedBlink(int rate);

void dataLedOn();
void dataLedOff();
void dataLedBlink(int rate);

void gnssLedOn();
void gnssLedOff();
void gnssLedBlink(int rate);

void functionKeyLedOn();
void functionKeyLedOff();
void functionKeyLedBlink(int rate);

void ledStatusUpdate();
void PowerControlTask();
