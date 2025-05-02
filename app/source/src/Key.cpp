#include <Arduino.h>
#include <global.h>
#include <settings.h>

#include <Key.h>
// 按键事件回调函数
EnhancedButton functionKey(150, 1000, 200, 300);

void buttonEventCallback(EnhancedButton *btn, EnhancedButton::Event_t event)
{
  switch (event)
  {
  case EnhancedButton::EVENT_SINGLE_CLICK:
    LOG_INFO("single click");
    // 单击LED亮一下后熄灭
    chargeLedBlink(100);
    break;

  case EnhancedButton::EVENT_DOUBLE_CLICK:
    LOG_INFO("double click");
    // 双击LED快速闪烁3次

      chargeLedBlink(1000);
    
    break;

  case EnhancedButton::EVENT_LONG_PRESS_START:
    LOG_INFO("long press start");
    chargeLedBlink(0);
    break;

  case EnhancedButton::EVENT_LONG_PRESS_HOLD:
    LOG_INFO("long press hold"); // 长按保持时LED闪烁
    break;

  case EnhancedButton::EVENT_LONG_PRESS_STOP:
    LOG_INFO("long press stop");
    break;

  case EnhancedButton::EVENT_RELEASED:
    // 此处不处理释放事件，因为其他事件已经处理了LED的状态
    // 如需要额外的释放处理可以在这里添加
    break;
  case EnhancedButton::_EVENT_LAST:
    break;

  default:
    break;
  }
}

void keyInit()
{
  pinMode(FunctionKey_PIN, INPUT_PULLUP);
  // 注册按键事件回调
  functionKey.attachCallback(buttonEventCallback);
}
void KeyMonitor(void *e)
{
	while(1)
	{
		functionKey.update(!digitalRead(FunctionKey_PIN));
		rt_thread_mdelay(15);
	}
}
