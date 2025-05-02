#pragma once
#include <stdint.h>

#include <Arduino.h>

typedef struct _present_device
{
    bool i2c = true;
    bool spi = true;
    bool bq40z50 = true;
    bool mp2762 = true;
    bool oled = false;
    bool adc = true;
    bool watchDog = true;
    bool button = true;
}present_device;

typedef struct _online_device
{
    bool i2c = false;
    bool i2c_slave = false;
    bool spi = false;
    bool bq40z50 = false;
    bool mp2762 = false;
    bool oled = false;
    bool adc = false;
    bool watchDog = false;
    bool button = false;
}online_device;

typedef struct tag_SystemParameter_s
{
	  unsigned char hw_version[8];
	  unsigned char sw_version[8];
	  unsigned short sound_status;
	  unsigned char reset_flag;
	  unsigned char poweroff_flag;
	  unsigned char record_flag;
	  unsigned char record_op;
	  unsigned char voice_flag;
	  unsigned char usb_power_flag;
}SystemParameter;

enum chargeStatus
{
    notCharge = 0,
    normalCharge,
    fastCharge,
};

typedef struct _ledState{
    bool isOn;
    uint32_t lastToggleTime;
    int currentRate;
} ledState;

extern ledState powerLed;
extern ledState chargerLed;
extern ledState functionKeyLed;
extern ledState gnssLed;
extern ledState dataLed;

extern uint8_t chargeStatus;

extern present_device present_devices;
extern online_device online_devices;
extern SystemParameter DisplayPannelParameter;
