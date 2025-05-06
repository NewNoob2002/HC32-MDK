#include <Arduino.h>
#include <rtthread.h>
#include <rthw.h>
#include <global.h>
#include <Bq40z50.h>

#include "System.h"

#define LIST_FIND_OBJ_NR 8

static gaugeState_t gaugeState = READ_VOLTAGE;

void checkBatteryLevels()
{
    if (online_devices.bq40z50 == false)
        return;

switch (gaugeState)
        {
        case READ_VOLTAGE:
        {
            batteryVoltage = (bq40z50->getVoltageMv() / 1000.0);
            gaugeState = READ_CURRENT;
            break;
        }
        case READ_CURRENT:
        {
            batteryLevelPercent = bq40z50->getRelativeStateOfCharge();
            gaugeState = READ_TEMP;
            break;
        }
        case READ_TEMP:
        {
            batteryTempC = bq40z50->getTemperatureC();
            gaugeState = READ_POWER;
            break;
        }
        case READ_POWER:
        {
            batteryChargingPercentPerHour =
                (float)bq40z50->getBatteryChargingPercentPerHour();
            gaugeState = DONE;
            break;
        }
        case DONE:
        {
            gaugeState = READ_VOLTAGE;
            break;
        }
        }

    if (batteryLevelPercent == 0.0) {
        batteryLevelPercent = 50.0;
    } else if (batteryLevelPercent <= 99.80 && batteryVoltage >= 8.38) {
        batteryLevelPercent = 100.0;
    }
    return;
}

rt_inline void object_split(int len)
{
    while (len--) rt_kprintf("-");
}

typedef struct
{
    rt_list_t *list;
    rt_list_t **array;
    rt_uint8_t type;
    int nr;             /* input: max nr, can't be 0 */
    int nr_out;         /* out: got nr */
} list_get_next_t;

static void list_find_init(list_get_next_t *p, rt_uint8_t type, rt_list_t **array, int nr)
{
    struct rt_object_information *info;
    rt_list_t *list;

    info = rt_object_get_information((enum rt_object_class_type)type);
    list = &info->object_list;

    p->list = list;
    p->type = type;
    p->array = array;
    p->nr = nr;
    p->nr_out = 0;
}

static rt_list_t *list_get_next(rt_list_t *current, list_get_next_t *arg)
{
    int first_flag = 0;
    rt_ubase_t level;
    rt_list_t *node, *list;
    rt_list_t **array;
    int nr;

    arg->nr_out = 0;

    if (!arg->nr || !arg->type)
    {
        return (rt_list_t *)RT_NULL;
    }

    list = arg->list;

    if (!current) /* find first */
    {
        node = list;
        first_flag = 1;
    }
    else
    {
        node = current;
    }

    level = rt_hw_interrupt_disable();

    if (!first_flag)
    {
        struct rt_object *obj;
        /* The node in the list? */
        obj = rt_list_entry(node, struct rt_object, list);
        if ((obj->type & ~RT_Object_Class_Static) != arg->type)
        {
            rt_hw_interrupt_enable(level);
            return (rt_list_t *)RT_NULL;
        }
    }

    nr = 0;
    array = arg->array;
    while (1)
    {
        node = node->next;

        if (node == list)
        {
            node = (rt_list_t *)RT_NULL;
            break;
        }
        nr++;
        *array++ = node;
        if (nr == arg->nr)
        {
            break;
        }
    }

    rt_hw_interrupt_enable(level);
    arg->nr_out = nr;
    return node;
}


void PrintfTaskInfo(void)
{
    rt_ubase_t level;
    list_get_next_t find_arg;
    rt_list_t *obj_list[LIST_FIND_OBJ_NR];
    rt_list_t *next = (rt_list_t*)RT_NULL;
    const char *item_title = "thread";
    int maxlen;

    list_find_init(&find_arg, RT_Object_Class_Thread, obj_list, sizeof(obj_list)/sizeof(obj_list[0]));

    maxlen = RT_NAME_MAX;

#ifdef RT_USING_SMP
    rt_kprintf("%-*.s cpu pri  status      sp     stack size max used left tick  error\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " --- ---  ------- ---------- ----------  ------  ---------- ---\n");
#else
    rt_kprintf("%-*.s pri  status      sp     stack size max used left tick  error\n", maxlen, item_title); object_split(maxlen);
    rt_kprintf(     " ---  ------- ---------- ----------  ------  ---------- ---\n");
#endif /*RT_USING_SMP*/

    do
    {
        next = list_get_next(next, &find_arg);
        {
            int i;
            for (i = 0; i < find_arg.nr_out; i++)
            {
                struct rt_object *obj;
                struct rt_thread thread_info, *thread;

                obj = rt_list_entry(obj_list[i], struct rt_object, list);
                level = rt_hw_interrupt_disable();

                if ((obj->type & ~RT_Object_Class_Static) != find_arg.type)
                {
                    rt_hw_interrupt_enable(level);
                    continue;
                }
                /* copy info */
                memcpy(&thread_info, obj, sizeof thread_info);
                rt_hw_interrupt_enable(level);

                thread = (struct rt_thread*)obj;
                {
                    rt_uint8_t stat;
                    rt_uint8_t *ptr;

#ifdef RT_USING_SMP
                    if (thread->oncpu != RT_CPU_DETACHED)
                        rt_kprintf("%-*.*s %3d %3d ", maxlen, RT_NAME_MAX, thread->name, thread->oncpu, thread->current_priority);
                    else
                        rt_kprintf("%-*.*s N/A %3d ", maxlen, RT_NAME_MAX, thread->name, thread->current_priority);

#else
                    rt_kprintf("%-*.*s %3d ", maxlen, RT_NAME_MAX, thread->name, thread->current_priority);
#endif /*RT_USING_SMP*/
                    stat = (thread->stat & RT_THREAD_STAT_MASK);
                    if (stat == RT_THREAD_READY)        rt_kprintf(" ready  ");
                    else if (stat == RT_THREAD_SUSPEND) rt_kprintf(" suspend");
                    else if (stat == RT_THREAD_INIT)    rt_kprintf(" init   ");
                    else if (stat == RT_THREAD_CLOSE)   rt_kprintf(" close  ");
                    else if (stat == RT_THREAD_RUNNING) rt_kprintf(" running");

#if defined(ARCH_CPU_STACK_GROWS_UPWARD)
                    ptr = (rt_uint8_t *)thread->stack_addr + thread->stack_size - 1;
                    while (*ptr == '#')ptr --;

                    rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\n",
                            ((rt_ubase_t)thread->sp - (rt_ubase_t)thread->stack_addr),
                            thread->stack_size,
                            ((rt_ubase_t)ptr - (rt_ubase_t)thread->stack_addr) * 100 / thread->stack_size,
                            thread->remaining_tick,
                            thread->error);
#else
                    ptr = (rt_uint8_t *)thread->stack_addr;
                    while (*ptr == '#')ptr ++;

                    rt_kprintf(" 0x%08x 0x%08x    %02d%%   0x%08x %03d\n",
                            thread->stack_size + ((rt_ubase_t)thread->stack_addr - (rt_ubase_t)thread->sp),
                            thread->stack_size,
                            (thread->stack_size - ((rt_ubase_t) ptr - (rt_ubase_t) thread->stack_addr)) * 100
                            / thread->stack_size,
                            thread->remaining_tick,
                            thread->error);
#endif
                }
            }
        }
    }
    while (next != (rt_list_t*)RT_NULL);

    return;
}

