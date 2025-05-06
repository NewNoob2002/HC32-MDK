/**
 *******************************************************************************
 * @file  gpio/gpio_output/source/main.c
 * @brief Main program of GPIO for the Device Driver Library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "main.h"
#include "charger_control.h"
#include "power_key.h"
#include "battery_i2c.h"

/**
 * @addtogroup HC32F460_DDL_Examples
 * @{
 */
unsigned int Charger_Monitor_Count = 0;
extern SystemParameter DisplayPannelParameter;
/*******************************************************************************
 * Function implementation - global ('extern') and local ('static')
 ******************************************************************************/
/**
 * @brief  Init
 * @param  None
 * @retval None
 */
void Charger_Control_GPIO_Init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(CHARGER_CTRL_PORT, CHARGER_CTRL_PIN1, &stcGpioInit);
	
	(void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(CHARGER_CTRL_PORT, CHARGER_CTRL_PIN2, &stcGpioInit);

	(void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_IN;
    (void)GPIO_Init(CHARGER_POWER_DET_PORT, CHARGER_POWER_DET_PIN, &stcGpioInit);
	
    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(CHARGER_ENABLE_PORT, CHARGER_ENABLE_PIN, &stcGpioInit);	
	///GPIO_ResetPins(CHARGER_ENABLE_PORT, CHARGER_ENABLE_PIN);
	GPIO_SetPins(CHARGER_ENABLE_PORT, CHARGER_ENABLE_PIN);
	
	(void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
    (void)GPIO_Init(CHARGER_SWITCH_PORT, CHARGER_SWITCH_PIN, &stcGpioInit);	
	GPIO_SetPins(CHARGER_SWITCH_PORT, CHARGER_SWITCH_PIN);
}

void USB_Switch_GPIO_Init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
	stcGpioInit.u16PullUp = PIN_PU_ON;
    (void)GPIO_Init(USB_SWITCH_PORT, USB_SWITCH_PIN, &stcGpioInit);
	GPIO_ResetPins(USB_SWITCH_PORT, USB_SWITCH_PIN);
}

void USB_Switch_GPIO_Control(unsigned char sw)
{
	if (sw)
		GPIO_SetPins(USB_SWITCH_PORT, USB_SWITCH_PIN);
	else
		GPIO_ResetPins(USB_SWITCH_PORT, USB_SWITCH_PIN);
}
		
void Charge_Enable_Switch(unsigned char on_off)
{
#if 1
	  if (!on_off)
			GPIO_ResetPins(CHARGER_ENABLE_PORT, CHARGER_ENABLE_PIN);
		else
			GPIO_SetPins(CHARGER_ENABLE_PORT, CHARGER_ENABLE_PIN);
#endif
}

void Charge_Current_Select(unsigned short select)
{
	  if (500 == select)
			GPIO_ResetPins(CHARGER_SWITCH_PORT, CHARGER_SWITCH_PIN);
	  else if (3000 == select)
			GPIO_SetPins(CHARGER_SWITCH_PORT, CHARGER_SWITCH_PIN);
	  else
		    GPIO_ResetPins(CHARGER_SWITCH_PORT, CHARGER_SWITCH_PIN);		  
}

extern BatteryParameter BAT_Parameter;
void Charger_Control_Monitor(void)
{
	unsigned char value1 = 0;
	unsigned char value2 = 0;
	static char on_off = 0;
		
	value1 = GPIO_ReadInputPins(CHARGER_CTRL_PORT, CHARGER_CTRL_PIN1);
	value2 = GPIO_ReadInputPins(CHARGER_POWER_DET_PORT, CHARGER_POWER_DET_PIN);
	if (!value2)
		DisplayPannelParameter.usb_power_flag = 0;
	
	if (value1)
		Charger_Monitor_Count++;
	else
		Charger_Monitor_Count = 0;
	
	if (Charger_Monitor_Count > 3)
	{
		Charge_Current_Select(3000);
		Charge_Enable_Switch(1);
		
		if (BAT_Parameter.EQ < 100)
		{
			on_off = ~on_off;
		    Charge_Led_Switch(on_off);
		}
		else if (BAT_Parameter.EQ < 101)
		{
			Charge_Led_Switch(1);
		}
		Power_Led_Switch(0);
		
		if (value2)
		{
			DisplayPannelParameter.usb_power_flag = 1;
		}
	}
	else
	{
		Charge_Enable_Switch(0);
		Charge_Led_Switch(0);
		Power_Led_Switch(1);
	}
}


 