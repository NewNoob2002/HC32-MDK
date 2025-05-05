#include <rtthread.h>
#include <rthw.h>
#include <HardwareSerial.h>

#include <usart.h>

void usart_init(uint32_t baud)
{
    Serial.begin(baud);
}

void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++) {
        if (*(str + i) == '\n') {
            Serial.print(a);
        }
        Serial.print(*(str + i));
    }
}

//char rt_hw_console_getchar(void)
//{
//    int ch = -1;

//    if (Serial.available())
//        ch = Serial.read();
//    else
//        rt_thread_mdelay(10);

//    return ch;
//}