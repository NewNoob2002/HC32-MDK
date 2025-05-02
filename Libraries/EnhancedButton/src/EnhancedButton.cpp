/**
  ******************************************************************************
  * @file    EnhancedButton.cpp
  * @brief   增强版按键事件库实现
  ******************************************************************************
  */

#include "EnhancedButton.h"

#ifdef ARDUINO
#  include "Arduino.h"
#  define GET_TICK() millis()
#endif

#ifndef GET_TICK
#  error "请定义GET_TICK()函数以获取系统时钟"
#endif

#ifndef UINT32_MAX
#  define UINT32_MAX  4294967295u
#endif

// 消抖阈值（连续几次读取相同才认为稳定）
#define DEBOUNCE_THRESHOLD 3

/**
 * @brief  按键构造函数
 */
EnhancedButton::EnhancedButton(
    uint16_t singleClickDelay,
    uint16_t longPressTime,
    uint16_t longPressHoldTime,
    uint16_t doubleClickTime
)
{
    // 初始化所有状态变量
    isPressed = false;
    isLongPressed = false;
    isTriggered = false;
    
    // 初始化内部数据结构
    data.state = STATE_IDLE;
    data.singleClickDelayCfg = singleClickDelay;
    data.longPressTimeCfg = longPressTime;
    data.longPressHoldTimeCfg = longPressHoldTime;
    data.doubleClickTimeCfg = doubleClickTime;
    
    data.pressTime = 0;
    data.releaseTime = 0;
    data.lastLongPressTime = 0;
    
    data.clickCount = 0;
    data.pressed = false;
    data.debounceCount = 0;
    
    data.lastEvent = EVENT_NONE;
    data.callback = nullptr;
}

/**
 * @brief  绑定事件回调函数
 */
void EnhancedButton::attachCallback(EventCallback_t callback)
{
    data.callback = callback;
}

/**
 * @brief  获取时间差值（带溢出处理）
 */
uint32_t EnhancedButton::getTickElapsed(uint32_t prevTick)
{
    uint32_t currentTick = GET_TICK();
    
    if (currentTick >= prevTick)
    {
        return currentTick - prevTick;
    }
    else
    {
        return UINT32_MAX - prevTick + 1 + currentTick;
    }
}

/**
 * @brief  触发事件
 */
void EnhancedButton::triggerEvent(Event_t event)
{
    data.lastEvent = event;
    isTriggered = true;
    
    if (data.callback != nullptr)
    {
        data.callback(this, event);
    }
}

/**
 * @brief  更新按键状态
 */
void EnhancedButton::update(bool isPress)
{
    isTriggered = false;
    
    // 更新按键信号，带消抖
    if (isPress != data.pressed)
    {
        data.debounceCount++;
        if (data.debounceCount >= DEBOUNCE_THRESHOLD)
        {
            data.pressed = isPress;
            data.debounceCount = 0;
        }
    }
    else
    {
        data.debounceCount = 0;
    }
    
    // 按键状态机
    switch (data.state)
    {
    case STATE_IDLE:
        if (data.pressed)
        {
            // 按下，记录时间并进入判断状态
            isPressed = true;
            data.pressTime = GET_TICK();
            data.state = STATE_WAIT_LONG_PRESS;
            // 触发按下事件
            triggerEvent(EVENT_PRESSING);
        }
        break;
        
    case STATE_WAIT_LONG_PRESS:
        if (data.pressed)
        {
            // 仍在按下状态，检查是否到达长按时间
            if (getTickElapsed(data.pressTime) >= data.longPressTimeCfg)
            {
                // 长按触发
                isLongPressed = true;
                data.lastLongPressTime = GET_TICK();
                data.state = STATE_LONG_PRESSING;
                // 触发长按开始事件
                triggerEvent(EVENT_LONG_PRESS_START);
            }
            else
            {
                // 持续触发PRESSING事件
                triggerEvent(EVENT_PRESSING);
            }
        }
        else
        {
            // 按键已释放，记录时间
            isPressed = false;
            data.releaseTime = GET_TICK();
            
            // 检查是否是双击的第二次点击
            if (data.clickCount > 0 && getTickElapsed(data.releaseTime - data.doubleClickTimeCfg) < data.doubleClickTimeCfg * 2)
            {
                // 双击触发
                data.clickCount = 0;
                data.state = STATE_IDLE;
                // 触发双击事件
                triggerEvent(EVENT_DOUBLE_CLICK);
            }
            else
            {
                // 可能是单击，但需要等待确认不是双击的第一次点击
                data.clickCount++;
                data.state = STATE_WAIT_DECISION;
            }
            
            // 触发释放事件
            triggerEvent(EVENT_RELEASED);
        }
        break;
        
    case STATE_WAIT_DECISION:
        if (data.pressed)
        {
            // 在等待决策期间又一次按下，可能是双击的第二次点击
            isPressed = true;
            data.pressTime = GET_TICK();
            data.state = STATE_WAIT_LONG_PRESS;
            // 触发按下事件
            triggerEvent(EVENT_PRESSING);
        }
        else if (getTickElapsed(data.releaseTime) >= data.singleClickDelayCfg)
        {
            // 超过了等待第二次点击的时间，确认为单击
            data.clickCount = 0;
            data.state = STATE_IDLE;
            // 触发单击事件
            triggerEvent(EVENT_SINGLE_CLICK);
        }
        break;
        
    case STATE_LONG_PRESSING:
        if (data.pressed)
        {
            // 长按状态下持续检测
            if (getTickElapsed(data.lastLongPressTime) >= data.longPressHoldTimeCfg)
            {
                // 触发长按保持事件
                data.lastLongPressTime = GET_TICK();
                triggerEvent(EVENT_LONG_PRESS_HOLD);
            }
            // 持续触发PRESSING事件
            triggerEvent(EVENT_PRESSING);
        }
        else
        {
            // 长按释放
            isPressed = false;
            isLongPressed = false;
            data.state = STATE_IDLE;
            data.clickCount = 0;
            // 触发长按释放和普通释放事件
            triggerEvent(EVENT_LONG_PRESS_STOP);
            triggerEvent(EVENT_RELEASED);
        }
        break;
        
    default:
        // 未知状态，重置
        data.state = STATE_IDLE;
        break;
    }
}

/**
 * @brief  获取按键事件字符串描述
 */
const char* EnhancedButton::getEventString(Event_t event)
{
    static const char* eventStrings[] = {
        "EVENT_NONE",
        "EVENT_SINGLE_CLICK",
        "EVENT_DOUBLE_CLICK",
        "EVENT_LONG_PRESS_START",
        "EVENT_LONG_PRESS_HOLD",
        "EVENT_LONG_PRESS_STOP",
        "EVENT_PRESSING",
        "EVENT_RELEASED"
    };
    
    if (event >= 0 && event < _EVENT_LAST)
    {
        return eventStrings[event];
    }
    
    return "EVENT_UNKNOWN";
}

/**
 * @brief  获取上次事件类型
 */
EnhancedButton::Event_t EnhancedButton::getLastEvent()
{
    return data.lastEvent;
}

/**
 * @brief  重置按键状态
 */
void EnhancedButton::reset()
{
    isPressed = false;
    isLongPressed = false;
    isTriggered = false;
    
    data.state = STATE_IDLE;
    data.pressTime = 0;
    data.releaseTime = 0;
    data.lastLongPressTime = 0;
    data.clickCount = 0;
    data.lastEvent = EVENT_NONE;
} 