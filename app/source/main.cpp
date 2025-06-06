/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ll.h"
#include <Arduino.h>
#include "HardwareI2c.h"
#include "MillisTaskManager.h"
MillisTaskManager task;

/* unlock/lock peripheral */
#define EXAMPLE_PERIPH_WE               (LL_PERIPH_GPIO | LL_PERIPH_EFM | LL_PERIPH_FCG | \
                                         LL_PERIPH_PWC_CLK_RMU | LL_PERIPH_SRAM)
#define EXAMPLE_PERIPH_WP               (LL_PERIPH_EFM | LL_PERIPH_FCG | LL_PERIPH_SRAM)


static void led_blink()
{
		GPIO_TogglePins(GPIO_PORT_B, GPIO_PIN_14);
}
/**
 * @brief  Main function of SPI tx/rx dma project
 * @param  None
 * @retval int32_t return value, if needed
 */
int main(void)
{
    /* Peripheral registers write unprotected */
    LL_PERIPH_WE(EXAMPLE_PERIPH_WE);
    /* Configure BSP */
		clock_init();
    systick_init();
		
		pinMode(PB14, OUTPUT);
		Slave_Initialize();
    /* Peripheral registers write protected */
    LL_PERIPH_WP(EXAMPLE_PERIPH_WP);
    
    task.Register(led_blink, 100);
//    task.Register(dmaSend, 10);
    
    while(1) {
        task.Running(millis());
    }
}

/**
 * @}
 */

/**
 * @}
 */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
