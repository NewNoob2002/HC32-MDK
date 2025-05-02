#ifndef  TASK_H
#define  TASK_H

void ledStatusUpdateTask(void *e);
void i2c_slave_task(void *e);
void btReadTask(void *e);
#endif