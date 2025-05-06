#ifndef  TASK_H
#define  TASK_H

void idleFeedWatchdogTask(void);
void KeyMonitor(void *e);
void ledStatusUpdateTask(void *e);
void BatteryCheckTask(void *e);
void btReadTask(void *e);
#endif