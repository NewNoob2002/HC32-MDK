/**
  ******************************************************************************
  * @file    EnhancedButton.h
  * @brief   增强版按键事件库，每种按键动作只触发单一事件
  * @version V1.0.0
  * @date    2023-05-20
  ******************************************************************************
  */

#ifndef __ENHANCED_BUTTON_H
#define __ENHANCED_BUTTON_H

#include <stdint.h>

#define ARDUINO

class EnhancedButton
{
public:
    // 按键事件类型枚举
    typedef enum
    {
        EVENT_NONE,              // 无事件
        EVENT_SINGLE_CLICK,      // 单击事件（与双击互斥）
        EVENT_DOUBLE_CLICK,      // 双击事件
        EVENT_LONG_PRESS_START,  // 长按开始事件
        EVENT_LONG_PRESS_HOLD,   // 长按保持事件（持续触发）
        EVENT_LONG_PRESS_STOP,   // 长按结束事件
        EVENT_PRESSING,          // 按下中事件（持续触发）
        EVENT_RELEASED,          // 释放事件
        _EVENT_LAST
    } Event_t;

    // 回调函数类型
    typedef void(*EventCallback_t)(EnhancedButton* btn, Event_t event);

    // 公开状态标志
    bool isPressed;              // 是否处于按下状态
    bool isLongPressed;          // 是否处于长按状态
    bool isTriggered;            // 是否触发了任何事件

public:
    /**
     * @brief  按键构造函数
     * @param  singleClickDelay: 单击判定延迟(ms)，在此延迟内未检测到第二次按下则判定为单击
     * @param  longPressTime: 长按触发时间(ms)
     * @param  longPressHoldTime: 长按保持触发间隔(ms)
     * @param  doubleClickTime: 双击间隔时间(ms)
     */
    EnhancedButton(
        uint16_t singleClickDelay = 250,
        uint16_t longPressTime = 1000,
        uint16_t longPressHoldTime = 200,
        uint16_t doubleClickTime = 300
    );

    /**
     * @brief  绑定事件回调函数
     * @param  callback: 回调函数指针
     */
    void attachCallback(EventCallback_t callback);

    /**
     * @brief  更新按键状态，必须放在主循环中周期性调用
     * @param  isPress: 当前按键状态，true为按下，false为释放
     */
    void update(bool isPress);

    /**
     * @brief  获取按键事件字符串描述
     * @param  event: 事件类型
     * @return 事件类型字符串
     */
    const char* getEventString(Event_t event);

    /**
     * @brief  获取上次事件类型
     * @return 事件类型
     */
    Event_t getLastEvent();

    /**
     * @brief  重置按键状态
     */
    void reset();

private:
    // 按键内部状态枚举
    typedef enum
    {
        STATE_IDLE,              // 空闲状态
        STATE_DEBOUNCE,          // 消抖状态
        STATE_WAIT_DECISION,     // 等待决策状态（单击或双击）
        STATE_WAIT_LONG_PRESS,   // 等待长按状态
        STATE_LONG_PRESSING,     // 长按中状态
    } State_t;

    // 内部私有变量结构体
    struct
    {
        State_t state;           // 当前状态
        
        uint16_t singleClickDelayCfg;    // 单击延迟配置
        uint16_t longPressTimeCfg;       // 长按触发时间配置
        uint16_t longPressHoldTimeCfg;   // 长按保持触发间隔配置
        uint16_t doubleClickTimeCfg;     // 双击间隔时间配置
        
        uint32_t pressTime;      // 按下时间戳
        uint32_t releaseTime;    // 释放时间戳
        uint32_t lastLongPressTime; // 上次长按触发时间戳
        
        uint8_t clickCount;      // 点击次数计数
        bool pressed;            // 按键物理状态
        uint8_t debounceCount;   // 消抖计数器
        
        Event_t lastEvent;       // 上次触发的事件
        EventCallback_t callback; // 事件回调函数
    } data;

    // 获取时间差值（带溢出处理）
    uint32_t getTickElapsed(uint32_t prevTick);
    
    // 触发事件
    void triggerEvent(Event_t event);
};

#endif /* __ENHANCED_BUTTON_H */ 