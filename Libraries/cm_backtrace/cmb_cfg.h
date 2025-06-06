/*
 * This file is part of the CmBacktrace Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: It is the configure head file for this library.
 * Created on: 2016-12-15
 */

#ifndef _CMB_CFG_H_
#define _CMB_CFG_H_

<<<<<<< HEAD
#ifdef	CMB_USER_CFG
#include "cmb_user_cfg.h"
#else
#ifdef __cplusplus
extern "C"
{
#endif
void cmb_printf(const char *__restrict __format, ...);
=======
#include <usart.h>
>>>>>>> f6f97cb766ab510a21feca370b07245adc777d6c

#ifdef	CMB_USER_CFG
#include "cmb_user_cfg.h"
#else
/* print line, must config by user */
<<<<<<< HEAD
#define cmb_println(...)               cmb_printf(__VA_ARGS__);cmb_printf("\r\n")
//#define cmb_println(...)               /* e.g., printf(__VA_ARGS__);printf("\r\n")  or  SEGGER_RTT_printf(0, __VA_ARGS__);SEGGER_RTT_WriteString(0, "\r\n")  */
=======
#define cmb_println(...)                cmb_printf(__VA_ARGS__);cmb_printf("\r\n")/* e.g., printf(__VA_ARGS__);printf("\r\n")  or  SEGGER_RTT_printf(0, __VA_ARGS__);SEGGER_RTT_WriteString(0, "\r\n")  */
>>>>>>> f6f97cb766ab510a21feca370b07245adc777d6c
/* enable bare metal(no OS) platform */
/* #define CMB_USING_BARE_METAL_PLATFORM */
#define CMB_USING_BARE_METAL_PLATFORM
/* enable OS platform */
/* #define CMB_USING_OS_PLATFORM */
/* OS platform type, must config when CMB_USING_OS_PLATFORM is enable */
/* #define CMB_OS_PLATFORM_TYPE           CMB_OS_PLATFORM_RTT or CMB_OS_PLATFORM_UCOSII or CMB_OS_PLATFORM_UCOSIII or CMB_OS_PLATFORM_FREERTOS or CMB_OS_PLATFORM_RTX5 or CMB_OS_PLATFORM_THREADX */
/* cpu platform type, must config by user */
#define CMB_CPU_PLATFORM_TYPE          CMB_CPU_ARM_CORTEX_M4/* CMB_CPU_ARM_CORTEX_M0 or CMB_CPU_ARM_CORTEX_M3 or CMB_CPU_ARM_CORTEX_M4 or CMB_CPU_ARM_CORTEX_M7 or CMB_CPU_ARM_CORTEX_M33 */
/* enable dump stack information */
/* #define CMB_USING_DUMP_STACK_INFO */
#define CMB_USING_DUMP_STACK_INFO
/* language of print information */
#define CMB_PRINT_LANGUAGE             CMB_PRINT_LANGUAGE_ENGLISH
/* #define CMB_PRINT_LANGUAGE             CMB_PRINT_LANGUAGE_ENGLISH(default) or CMB_PRINT_LANGUAGE_CHINESE or CMB_PRINT_LANGUAGE_CHINESE_UTF8 */
#endif

<<<<<<< HEAD
#ifdef __cplusplus
}
#endif

=======
>>>>>>> f6f97cb766ab510a21feca370b07245adc777d6c
#endif /* _CMB_CFG_H_ */
