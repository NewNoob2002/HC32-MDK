/*********************
 *      INCLUDES
 *********************/
#ifndef CHARGER_CONTROL_H
#define CHARGER_CONTROL_H

#define CHARGER_ENABLE_PORT          (GPIO_PORT_A)
#define CHARGER_ENABLE_PIN           (GPIO_PIN_04)

#define CHARGER_CTRL_PORT            (GPIO_PORT_B)
#define CHARGER_CTRL_PIN1            (GPIO_PIN_07)           ///1.65V
#define CHARGER_CTRL_PIN2            (GPIO_PIN_06)           ///3.23V

#define CHARGER_SWITCH_PORT          (GPIO_PORT_B)
#define CHARGER_SWITCH_PIN           (GPIO_PIN_10)

#define USB_SWITCH_PORT              (GPIO_PORT_B)
#define USB_SWITCH_PIN               (GPIO_PIN_08)

#define CHARGER_POWER_DET_PORT       (GPIO_PORT_A)
#define CHARGER_POWER_DET_PIN        (GPIO_PIN_01)

void Charger_Control_GPIO_Init(void);
void Charge_Enable_Switch(unsigned char on_off);
void Charger_Control_Monitor(void);
void USB_Switch_GPIO_Init(void);
void USB_Switch_GPIO_Control(unsigned char sw);
void Charge_Current_Select(unsigned short select);
#endif