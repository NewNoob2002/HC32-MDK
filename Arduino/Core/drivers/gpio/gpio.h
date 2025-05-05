#pragma once
#include "hc32_ll.h"
#include "addon_gpio.h"
#include "adc.h"
#include "WVariant.h"
#include "core_types.h"
#include "core_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// GPIO wrappers for PORT_* functions
//
#define PIN_ARG(gpio_pin) PIN_MAP[gpio_pin].port, PIN_MAP[gpio_pin].bit_mask()

/**
 * @brief GPIO wrapper for PORT_Init
 */
inline int32_t _GPIO_Init(gpio_pin_t gpio_pin, const stc_gpio_init_t *pstcPortInit)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_Init");
    return GPIO_Init(PIN_ARG(gpio_pin), pstcPortInit);
}

/**
 * @brief GPIO wrapper for PORT_GetConfig
 */
inline int32_t GPIO_GetConfig(gpio_pin_t gpio_pin, stc_gpio_init_t *pstcPortInit)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_GetConfig");
    return PORT_GetConfig(PIN_ARG(gpio_pin), pstcPortInit);
}

/**
 * @brief GPIO wrapper for PORT_GetBit
 */
inline en_pin_state_t GPIO_GetBit(gpio_pin_t gpio_pin)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_GetBit");
    return GPIO_ReadInputPins(PIN_ARG(gpio_pin));
}

//    /**
//     * @brief GPIO wrapper for PORT_OE
//     */
//    inline en_result_t GPIO_OE(gpio_pin_t gpio_pin, en_functional_state_t enNewState)
//    {
//        ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_OE");
//        return PORT_OE(PIN_ARG(gpio_pin), enNewState);
//    }

/**
 * @brief GPIO wrapper for PORT_SetBits
 */
inline void GPIO_SetBits(gpio_pin_t gpio_pin)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_SetBits");
    GPIO_SetPins(PIN_ARG(gpio_pin));
}

/**
 * @brief GPIO wrapper for PORT_ResetBits
 */
inline void GPIO_ResetBits(gpio_pin_t gpio_pin)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_ResetBits");
    return GPIO_ResetPins(PIN_ARG(gpio_pin));
}

/**
 * @brief GPIO wrapper for PORT_Toggle
 */
inline void GPIO_Toggle(gpio_pin_t gpio_pin)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_Toggle");
    GPIO_TogglePins(PIN_ARG(gpio_pin));
}

/**
 * @brief GPIO wrapper for PORT_SetFunc
 * @param enFuncSelect GPIO pin primary function select
 * @param enSubFunc GPIO pin sub-function enable/disable (subfunction is GPIO output for most pins)
 */
inline void GPIO_SetFunction(gpio_pin_t gpio_pin, uint16_t enFuncSelect, en_functional_state_t enSubFunc = DISABLE, uint8_t u8SubFunc = 0)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_SetFunc");
    GPIO_SetFunc(PIN_ARG(gpio_pin), enFuncSelect);
    if (enSubFunc == ENABLE) {
        if (u8SubFunc == 0)
            CORE_DEBUG_PRINTF("Enabled SubFunction But not set SubFunction Sel");
    }
}

/**
 * @brief GPIO wrapper for PORT_GetFunc
 * @param enFuncSelect GPIO pin primary function select
 * @param enSubFunc GPIO pin sub-function enable/disable (subfunction is GPIO output for most pins)
 */
inline int32_t GPIO_GetFunc(gpio_pin_t gpio_pin, uint16_t *enFuncSelect, en_functional_state_t *enSubFunc)
{
    ASSERT_GPIO_PIN_VALID(gpio_pin, "GPIO_GetFunc");
    return PORT_GetFunc(PIN_ARG(gpio_pin), enFuncSelect, enSubFunc);
}

#ifdef __cplusplus
}
#endif
