/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-12     Bernard      first version
 * 2006-05-27     Bernard      add support for same priority thread schedule
 * 2006-08-10     Bernard      remove the last rt_schedule in rt_tick_increase
 * 2010-03-08     Bernard      remove rt_passed_second
 * 2010-05-20     Bernard      fix the tick exceeds the maximum limits
 * 2010-07-13     Bernard      fix rt_tick_from_millisecond issue found by kuronca
 * 2011-06-26     Bernard      add rt_tick_set function.
 * 2018-11-22     Jesven       add per cpu tick
 */

#include <rthw.h>
#include <rtthread.h>
#include <hc32_ll.h>
#include "systick.h"
#include "delay.h"

static rt_tick_t rt_tick = 0;

/**
 * This function will initialize system tick and set it to zero.
 * @ingroup SystemInit
 *
 * @deprecated since 1.1.0, this function does not need to be invoked
 * in the system initialization.
 */
void rt_system_tick_init(void)
{
}

/**
 * @addtogroup Clock
 */

/**@{*/

/**
 * This function will return current tick from operating system startup
 *
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* return the global tick */
    return rt_tick;
}

/**
 * This function will set current tick
 */
void rt_tick_set(rt_tick_t tick)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    rt_tick = tick;
    rt_hw_interrupt_enable(level);
}

/**
 * This function will notify kernel there is one tick passed. Normally,
 * this function is invoked by clock ISR.
 */
void rt_tick_increase(void)
{
    struct rt_thread *thread;

    /* increase the global tick */
    ++ rt_tick;

    /* check time slice */
    thread = rt_thread_self();

    -- thread->remaining_tick;
    if (thread->remaining_tick == 0)
    {
        /* change to initialized tick */
        thread->remaining_tick = thread->init_tick;

        /* yield */
        rt_thread_yield();
    }

    /* check timer */
    rt_timer_check();
}

/**
 * This function will calculate the tick from millisecond.
 *
 * @param ms the specified millisecond
 *           - Negative Number wait forever
 *           - Zero not wait
 *           - Max 0x7fffffff
 *
 * @return the calculated tick
 */
rt_tick_t rt_tick_from_millisecond(rt_int32_t ms)
{
    rt_tick_t tick;

    if (ms < 0)
    {
        tick = (rt_tick_t)RT_WAITING_FOREVER;
    }
    else
    {
        tick = RT_TICK_PER_SECOND * (ms / 1000);
        tick += (RT_TICK_PER_SECOND * (ms % 1000) + 999) / 1000;
    }

    /* return the calculated tick */
    return tick;
}

/**@}*/
uint32_t millis()
{
  return rt_tick;
}

uint32_t micros()
{
  // based on implementation by STM32duino
  // https://github.com/stm32duino/Arduino_Core_STM32/blob/586319c6c2cee268747c8826d93e84b26d1549fd/libraries/SrcWrapper/src/stm32/clock.c#L29

  // read systick counter value and millis counter value
  uint32_t ms = rt_tick;
  volatile uint32_t ticks = SysTick->VAL;

  // and again
  uint32_t ms2 = rt_tick;
  volatile uint32_t ticks2 = SysTick->VAL;

  // if ms != ms2, then a systick occurred between the two reads
  // and we must use ms2 and ticks2
  if (ms != ms2)
  {
    ms = ms2;
    ticks = ticks2;
  }

  // get ticks per ms
  const uint32_t ticks_per_ms = SysTick->LOAD + 1;

  // calculate microseconds
  return (ms * 1000) + (((ticks_per_ms - ticks) * 1000) / ticks_per_ms);
}

void delay_ms(uint32_t ms)
{
    uint32_t tickstart = rt_tick;
    uint32_t wait = ms / (1000 / RT_TICK_PER_SECOND);

    while((rt_tick - tickstart) < wait)
    {
    }
}

void delay_us(uint32_t us)
{
    uint32_t total = 0;
    uint32_t target =(SystemCoreClock / 1000000U) * us;
    int last = SysTick->VAL;
    int now = last;
    int diff = 0;
start:
    now = SysTick->VAL;
    diff = last - now;
    if(diff > 0)
    {
        total += diff;
    }
    else
    {
        total += diff + (SystemCoreClock / RT_TICK_PER_SECOND) ;
    }
    if(total > target)
    {
        return;
    }
    last = now;
    goto start;
}
