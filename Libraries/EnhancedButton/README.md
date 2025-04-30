# EnhancedButton

EnhancedButton是一个高级按键事件处理库，基于原始ButtonEvent库进行了增强，解决了按键事件重复触发的问题，实现了按键事件的互斥处理。

## 主要特点

- **事件互斥**: 单击、双击和长按事件互不干扰，只触发一种事件
- **延迟确认**: 单击事件会延迟触发，以确保不是双击的第一次点击
- **状态机实现**: 使用状态机设计，逻辑清晰
- **带消抖处理**: 内置按键消抖算法，提高事件检测稳定性
- **长按持续触发**: 支持长按开始、长按保持和长按结束三种状态

## 事件类型

库提供以下事件类型：

- `EVENT_SINGLE_CLICK`: 单击事件，确保不是双击的一部分
- `EVENT_DOUBLE_CLICK`: 双击事件
- `EVENT_LONG_PRESS_START`: 长按开始事件，当按键按下时间超过设定的长按阈值触发
- `EVENT_LONG_PRESS_HOLD`: 长按保持事件，长按期间周期性触发
- `EVENT_LONG_PRESS_STOP`: 长按结束事件，长按释放时触发
- `EVENT_PRESSING`: 按键按下状态事件，按下期间持续触发
- `EVENT_RELEASED`: 按键释放事件

## 使用方法

### 基本用法

```cpp
#include <EnhancedButton.h>

// 创建按键对象
// 参数: 单击判定延迟(ms), 长按时间(ms), 长按保持间隔(ms), 双击间隔(ms)
EnhancedButton button(250, 1000, 200, 300);

void setup() {
  // 设置按键回调函数
  button.attachCallback(onButtonEvent);
  
  // 配置按键引脚
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // 定时更新按键状态
  button.update(!digitalRead(BUTTON_PIN)); // 上拉输入需要取反
}

// 按键事件回调函数
void onButtonEvent(EnhancedButton* btn, EnhancedButton::Event_t event) {
  switch(event) {
    case EnhancedButton::EVENT_SINGLE_CLICK:
      // 处理单击事件
      break;
      
    case EnhancedButton::EVENT_DOUBLE_CLICK:
      // 处理双击事件
      break;
      
    case EnhancedButton::EVENT_LONG_PRESS_START:
      // 处理长按开始事件
      break;
      
    case EnhancedButton::EVENT_LONG_PRESS_HOLD:
      // 处理长按保持事件
      break;
      
    case EnhancedButton::EVENT_LONG_PRESS_STOP:
      // 处理长按结束事件
      break;
  }
}
```

### 与任务管理器结合使用

```cpp
#include <EnhancedButton.h>
#include <MillisTaskManager.h>

MillisTaskManager taskManager;
EnhancedButton button(250, 1000, 200, 300);

void setup() {
  button.attachCallback(onButtonEvent);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // 注册按键扫描任务
  taskManager.Register(buttonScan, 10); // 10ms扫描一次
}

void loop() {
  taskManager.Running(millis());
}

void buttonScan() {
  button.update(!digitalRead(BUTTON_PIN));
}
```

## 配置参数说明

创建EnhancedButton对象时可以配置以下参数：

1. **singleClickDelay**: 单击判定延迟时间(ms)，在此时间内如果没有第二次点击则确认为单击
2. **longPressTime**: 长按触发时间(ms)，按下超过此时间触发长按事件
3. **longPressHoldTime**: 长按保持触发间隔(ms)，长按状态下每隔此时间触发一次EVENT_LONG_PRESS_HOLD事件
4. **doubleClickTime**: 双击间隔时间(ms)，两次点击之间的最大时间间隔

## 与原ButtonEvent库的区别

1. 单击事件(`EVENT_SINGLE_CLICK`)与双击事件(`EVENT_DOUBLE_CLICK`)互斥
2. 长按事件分为三个阶段: 开始、保持和结束
3. 增加了消抖处理，提高了事件检测稳定性
4. 采用状态机重构，逻辑更清晰
5. 事件命名更符合直觉 