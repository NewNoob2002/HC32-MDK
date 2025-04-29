/**
 *******************************************************************************
 * @file  hc32_ll_utility.c
 * @brief This file provides utility functions for DDL.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
   2022-06-30       CDT             Support re-target printf for IAR EW version 9 or later
   2023-06-30       CDT             Modify register USART DR to USART TDR
                                    Prohibit DDL_DelayMS and DDL_DelayUS functions from being optimized
   2024-08-31       CDT             Optimized the Delay functions as cache is enabled
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
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
#include "hc32_ll_utility.h"

/**
 * @addtogroup LL_Driver
 * @{
 */

/**
 * @defgroup LL_UTILITY UTILITY
 * @brief DDL Utility Driver
 * @{
 */

#if (LL_UTILITY_ENABLE == DDL_ON)

/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/

/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/

/*******************************************************************************
 * Global variable definitions (declared in header file with 'extern')
 ******************************************************************************/

/*******************************************************************************
 * Local function prototypes ('static')
 ******************************************************************************/

/*******************************************************************************
 * Local variable definitions ('static')
 ******************************************************************************/
/**
 * @defgroup UTILITY_Local_Variables UTILITY Local Variables
 * @{
 */
#endif

/**
 * @}
 */

/**
 * @brief This function Initializes the interrupt frequency of the SysTick.
 * @param [in] u32Freq                  SysTick interrupt frequency (1 to 1000).
 * @retval int32_t:
 *           - LL_OK: SysTick Initializes succeed
 *           - LL_ERR: SysTick Initializes failed
 */
//int32_t SysTick_Init(uint32_t u32Freq)
//{
//    uint32_t i32Ret = 1;
//    if(0UL == SysTick_Config(SYSTICK_LOAD_VALUE))
//			i32Ret = 0;

//    return i32Ret;
//}

//void SysTick_Handler(void)
//{
//    SystemTickCount++;
//}

/**
  * @brief  Return the millis time after power
  * @param  None
  * @retval get mills
  */
//uint32_t millis(void)
//{
//    return SYSTICK_MILLIS;
//}

//uint32_t micros(void)
//{
//    return (SYSTICK_MILLIS * 1000 + (SYSTICK_LOAD_VALUE - SysTick->VAL) / CYCLES_PER_MICROSECOND);
//}

//void delay_ms(uint32_t ms)
//{
//    uint32_t tickstart = SystemTickCount;
//    uint32_t wait = ms / SYSTICK_TICK_INTERVAL;

//    while((SystemTickCount - tickstart) < wait)
//    {
//    }
//}

//void delay_us(uint32_t us)
//{
//    uint32_t total = 0;
//    uint32_t target = CYCLES_PER_MICROSECOND * us;
//    int last = SysTick->VAL;
//    int now = last;
//    int diff = 0;
//start:
//    now = SysTick->VAL;
//    diff = last - now;
//    if(diff > 0)
//    {
//        total += diff;
//    }
//    else
//    {
//        total += diff + SYSTICK_LOAD_VALUE;
//    }
//    if(total > target)
//    {
//        return;
//    }
//    last = now;
//    goto start;
//}

#ifdef __DEBUG
/**
 * @brief DDL assert error handle function
 * @param [in] file                     Point to the current assert the wrong file.
 * @param [in] line                     Point line assert the wrong file in the current.
 * @retval None
 */
__WEAKDEF void DDL_AssertHandler(const char *file, int line)
{
    /* Users can re-implement this function to print information */
    DDL_Printf("Wrong parameters value: file %s on line %d\r\n", file, line);

    for (;;) {
    }
}
#endif /* __DEBUG */

#if (LL_PRINT_ENABLE == DDL_ON)

#if (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) || \
    (defined (__ICCARM__) && (__VER__ < 9000000)) || (defined (__CC_ARM))
/**
 * @brief  Re-target fputc function.
 * @param  [in] ch
 * @param  [in] f
 * @retval int32_t
 */
int32_t fputc(int32_t ch, FILE *f)
{
    (void)f;  /* Prevent unused argument compilation warning */

    return (LL_OK == DDL_ConsoleOutputChar((char)ch)) ? ch : -1;
}

#elif (defined (__ICCARM__) && (__VER__ >= 9000000))
#include <LowLevelIOInterface.h>
#pragma module_name = "?__write"
size_t __dwrite(int handle, const unsigned char *buffer, size_t size)
{
    size_t nChars = 0;
    size_t i;

    if (buffer == NULL) {
        /*
         * This means that we should flush internal buffers.  Since we
         * don't we just return.  (Remember, "handle" == -1 means that all
         * handles should be flushed.)
         */
        return 0;
    }

    /* This template only writes to "standard out" and "standard err",
     * for all other file handles it returns failure. */
    if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
        return _LLIO_ERROR;
    }

    for (i = 0; i < size; i++) {
        if (DDL_ConsoleOutputChar((char)buffer[i]) < 0) {
            return _LLIO_ERROR;
        }

        ++nChars;
    }

    return nChars;
}

#elif defined (__GNUC__) && !defined (__CC_ARM)
/**
 * @brief  Re-target _write function.
 * @param  [in] fd
 * @param  [in] data
 * @param  [in] size
 * @retval int32_t
 */
int32_t _write(int fd, char data[], int32_t size)
{
    int32_t i = -1;

    if (NULL != data) {
        (void)fd;  /* Prevent unused argument compilation warning */

        for (i = 0; i < size; i++) {
            if (LL_OK != DDL_ConsoleOutputChar(data[i])) {
                break;
            }
        }
    }

    return i ? i : -1;
}
#endif

/**
 * @brief  Initialize printf function
 * @param  [in] vpDevice                Pointer to print device
 * @param  [in] u32Param                Print device parameter
 * @param  [in] pfnPreinit              The function pointer for initializing clock, port, print device etc.
 * @retval int32_t:
 *           - LL_OK:                   Initialize successfully.
 *           - LL_ERR:                  The callback function pfnPreinit occurs error.
 *           - LL_ERR_INVD_PARAM:       The pointer pfnPreinit is NULL.
 */
int32_t LL_PrintfInit(void *vpDevice, uint32_t u32Param, int32_t (*pfnPreinit)(void *vpDevice, uint32_t u32Param))
{
    int32_t i32Ret = LL_ERR_INVD_PARAM;

    if (NULL != pfnPreinit) {
        i32Ret = pfnPreinit(vpDevice, u32Param);   /* The callback function initialize clock, port, print device etc */
        if (LL_OK == i32Ret) {
            LL_SetPrintDevice(vpDevice);
            LL_SetPrintTimeout((u32Param == 0UL) ? 0UL : (HCLK_VALUE / u32Param));
        } else {
            i32Ret = LL_ERR;
            DDL_ASSERT(i32Ret == LL_OK);           /* Initialize unsuccessfully */
        }
    }

    return i32Ret;
}

/**
 * @brief  Transmit character.
 * @param  [in] cData                   The character for transmitting
 * @retval int32_t:
 *           - LL_OK:                   Transmit successfully.
 *           - LL_ERR_TIMEOUT:          Transmit timeout.
 *           - LL_ERR_INVD_PARAM:       The print device is invalid.
 */
__WEAKDEF int32_t DDL_ConsoleOutputChar(char cData)
{
    uint32_t u32TxEmpty = 0UL;
    __IO uint32_t u32TmpCount = 0UL;
    int32_t i32Ret = LL_ERR_INVD_PARAM;
    uint32_t u32Timeout = LL_GetPrintTimeout();
    CM_USART_TypeDef *USARTx = (CM_USART_TypeDef *)LL_GetPrintDevice();

    if (NULL != USARTx) {
        /* Wait TX data register empty */
        while ((u32TmpCount <= u32Timeout) && (0UL == u32TxEmpty)) {
            u32TxEmpty = READ_REG32_BIT(USARTx->SR, USART_SR_TXE);
            u32TmpCount++;
        }

        if (0UL != u32TxEmpty) {
            WRITE_REG16(USARTx->TDR, (uint16_t)cData);
            i32Ret = LL_OK;
        } else {
            i32Ret = LL_ERR_TIMEOUT;
        }
    }

    return i32Ret;
}

#endif /* LL_PRINT_ENABLE */


/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
