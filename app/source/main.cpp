/*******************************************************************************
 * Include files
 ******************************************************************************/
#include <Arduino.h>
#include <Bq40z50.h>
#include <Mp2762A.h>
#include <HardwareI2cSlave.h>
#include <Task.h>
#include <Wire.h>
#include <global.h>
#include <hc32_ll.h>
#include <settings.h>

// #include "MillisTaskManager.h"
// MillisTaskManager task;
/**
 * @addtogroup HC32F460_DDL_Examples
 * @{
 */
uint8_t chargeStatus = notCharge;
float batteryLevelPercent = 50; // SOC measured from fuel gauge, in %. Used in
                                // multiple places (display, serial debug, log)
float batteryVoltage = 0.0;
float batteryChargingPercentPerHour = 0.0;
float batteryTempC = 0.0;
present_device present_devices;
online_device online_devices;

// system device
unsigned short reg_value = 0;
static uint32_t lastPowerOnTime = 0;
SystemParameter DisplayPannelParameter;
/*******************************************************************************
 * Local type definitions ('typedef')
 ******************************************************************************/
static struct rt_thread keyCheck_thread;
static rt_uint8_t keyCheck_thread_stack[1024];
static rt_uint8_t keyCheck_thread_priority = 6;

static struct rt_thread led_thread;
static rt_uint8_t led_thread_stack[1024];
static rt_uint8_t led_thread_priority = 6;

static struct rt_thread message_thread;
static rt_uint8_t message_thread_stack[2048];
static rt_uint8_t message_thread_priority = 7;
/*******************************************************************************
 * Local pre-processor symbols/macros ('#define')
 ******************************************************************************/
/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE                                                      \
  (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_PWC_CLK_RMU |    \
   LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)

/**
 * @brief  Main function of SPI tx/rx dma project
 * @param  None
 * @retval int32_t return value, if needed
 */
int main(void) {
  /* Peripheral registers write unprotected */
  LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
  /* Configure BSP */
	PowerControlInit();
	adcInit();
	keyInit();
	ledInit();
  // start Init device
  unsigned short reg_value = *((unsigned short *)0x400540C0UL);
  if (reg_value & 0x0100U) {
    LOG_DEBUG("SoftReset");
		Power_Control_Pin_Switch(1);
    //        Power_Control_Pin_Switch(1);
  } else if (reg_value & 0x0002U) {
    LOG_DEBUG("EWDT or Hardware reset");
  } else if (reg_value & 0x2000U) {
    LOG_ERROR("XTAL error");
  }
//	Charger_Control_GPIO_Init();
//	USB_Switch_GPIO_Init();
  delay_ms(50);
  if(Wire.begin() == LL_OK)
			online_devices.i2c = true;
	bq40z50 = new BQ40Z50();
//	mp2762a = new MP2762A();
	if(bq40z50->begin())
	{
		LOG_INFO("Find Mp2762, 0x0B");
		online_devices.bq40z50 = true;
	}
	
	mp2762a->registerReset();
	mp2762a->setFastChargeVoltageMv(6600);
	mp2762a->setPrechargeCurrentMa(500);

  mp2762a->setFastChargeCurrentMa(1600);
      // get charge status
  mp2762a->getChargeStatus();
//	Wire.beginTransmission(0x0b);
//  if (Wire.endTransmission() == 0)
//	{
//		LOG_INFO("Find 0x0B");
//	}
//	Wire.beginTransmission(0x0b);
//	Wire.write(0x08);
//	if (Wire.endTransmission(false) != 0) //Send a restart command. Do not release bus.
//  {
//    //Sensor did not ACK
//    LOG_ERROR("Error: Sensor did not ack");
//  }
//	uint8_t buff[2];
//	if( Wire.requestFrom(0x0b, buff, 2) > 0)
//	{
//		LOG_INFO("got value :%d", buff[1] << 8 | buff[0]);
//	}
//	else
//		LOG_ERROR("cant get");
//	bq40z50 = new BQ40Z50();
//	bq40z50->begin();
//	
//	float remainingCapacity = bq40z50->getRemainingCapacityMah();
//	float fullCapacity = bq40z50->getFullChargeCapacityMah();
//	Serial.print("Remaining: ");
//	Serial.print(remainingCapacity);
//	Serial.print(" mAh, Full: ");
//	Serial.print(fullCapacity);
//	Serial.print(" mAh, Percentage: ");
//	
  //		LOG_ERROR("I2c init failed");
  //    I2C_Slave.begin();

  /* Peripheral registers write protected */
  LL_PERIPH_WP(EXAMPLE_PERIPH_WP);

  rt_thread_init(&led_thread, "led", ledStatusUpdateTask, RT_NULL,
                 &led_thread_stack, sizeof(led_thread_stack),
                 led_thread_priority, 100);
  //    rt_thread_init(&message_thread,
  //                   "message_task",
  //                   btReadTask,
  //                   RT_NULL,
  //                   &message_thread_stack,
  //                   sizeof(message_thread_stack),
  //                   message_thread_priority,
  //                   1000);
  rt_thread_init(&keyCheck_thread, "keyCheck", KeyMonitor, RT_NULL,
                 &keyCheck_thread_stack, sizeof(keyCheck_thread_stack),
                 keyCheck_thread_priority, 100);

  //    rt_thread_startup(&message_thread);
  rt_thread_startup(&led_thread);
  rt_thread_startup(&keyCheck_thread);
								 
	while(1)
	{
//	float remainingCapacity = bq40z50->getRemainingCapacityMah();
//	float fullCapacity = bq40z50->getFullChargeCapacityMah();
//	Serial.print("Remaining: ");
//	Serial.print(remainingCapacity);
//	Serial.print(" mAh, Full: ");
//	Serial.print(fullCapacity);
//	Serial.print(" mAh, Percentage: ");
//	float tempC = remainingCapacity / fullCapacity;
//	Serial.print(tempC, 4);
//	Serial.println("");
		uint16_t status = mp2762a->getChargeStatus();
		LOG_INFO("now charge status: %d", status);
		rt_thread_delay(1000);
	}
}
