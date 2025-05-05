#include <Arduino.h>
#include "adc.h"
//#include "../../yield.h"
#include "core_debug.h"
#include "debug.h"

/**
 * @brief assert that channel id is valid
 * @param device ADC device configuration
 * @param channel ADC channel id to check
 */
#define ASSERT_CHANNEL_ID(device, channel) \
    CORE_ASSERT(channel >= 0 && channel < device->adc.channel_count, "invalid channel id")

/**
 * @brief assert that adc device is initialized
 */
#define ASSERT_INITIALIZED(device, function_name) \
    CORE_ASSERT(device->state.initialized, "ADC device not initialized (calling " function_name ")")

/**
 * @brief adc register to debug name
 */
#define ADC_REG_TO_NAME(reg)  \
    reg == CM_ADC1   ? "ADC1" \
    : reg == CM_ADC2 ? "ADC2" \
                     : "N/A"

/**
 * @brief debug printf for ADC
 */
#define ADC_DEBUG_PRINTF(device, fmt, ...) \
    rt_kprintf("[%s] " fmt "\n", ADC_REG_TO_NAME(device->adc.register_base), ##__VA_ARGS__)

//
// ADC init
//
void adc_clock_init(bool select_systemClock = true)
{
	if(select_systemClock == true)
	{
		CLK_SetClockDiv((CLK_BUS_PCLK2 | CLK_BUS_PCLK4), (CLK_PCLK2_DIV8 | CLK_PCLK4_DIV2));
	}
	else
	{
		LOG_ERROR("XTAL have inited, please select systemClock as adc clk");
	}
	CLK_SetPeriClockSrc(CLK_PERIPHCLK_PCLK);
}
/**
 * @brief ADC peripheral init
 */
inline void adc_adc_init(const adc_device_t *device)
{
		stc_adc_init_t stcAdcInit;
		adc_clock_init();
    // enable ADC peripheral clock
    FCG_Fcg3PeriphClockCmd(device->adc.clock_id, ENABLE);

    // initialize ADC peripheral
    stcAdcInit = {
			  .u16ScanMode = device->init_params.scan_mode,
        .u16Resolution = device->init_params.resolution,
        .u16DataAlign = device->init_params.data_alignment,
    };
    ADC_Init(device->adc.register_base, &stcAdcInit);

//    // ADC will always trigger conversion by software
//    ADC_TriggerSrcCmd(device->adc.register_base, device->adc.sequence, Disable);
}

void adc_device_init(adc_device_t *device)
{
    // do nothing if ADC is already initialized
    if (device->state.initialized)
    {
        return;
    }

    // adc is set up to trigger conversion by software
    // adc_wait_for_conversion() waits until the ADC conversion is complete
    adc_adc_init(device);

    // set initialized flag
    device->state.initialized = true;
    // Use a direct print instead of the macro for this specific message to avoid formatting issues
    ADC_DEBUG_PRINTF(device, "initialized device");
}
//
// ADC Channel API
//

inline uint8_t adc_channel_to_mask(const adc_device_t *device, const uint8_t channel)
{
    ASSERT_CHANNEL_ID(device, channel);
    return channel;
}

void adc_enable_channel(const adc_device_t *device, const uint8_t adc_channel, uint8_t sample_time)
{
    ASSERT_INITIALIZED(device, STRINGIFY(adc_enable_channel));
    ASSERT_CHANNEL_ID(device, adc_channel);
    CORE_ASSERT(sample_time > 0, "adc channel sample_time must be > 0")

    ADC_DEBUG_PRINTF(device, "enable channel %d, sample_time=%d", adc_channel, sample_time);
	  /* 4.1 Set the ADC pin to analog input mode. */
//		pinMode(PA0, INPUT_ANALOG);
	
		uint8_t adc_channel_bit = adc_channel_to_mask(device, adc_channel);
	
		ADC_ChCmd(device->adc.register_base, device->adc.sequence, adc_channel_bit, ENABLE);
		ADC_SetSampleTime(device->adc.register_base, adc_channel_bit, sample_time);
	
	  ADC_ConvDataAverageConfig(device->adc.register_base, ADC_AVG_CNT8);
    ADC_ConvDataAverageChCmd(device->adc.register_base, adc_channel_bit, ENABLE);
}

void adc_disable_channel(const adc_device_t *device, const uint8_t adc_channel)
{
    if (!device->state.initialized)
    {
        // if adc is not initialized, it's safe to assume no channels have been enabled yet
        return;
    }

//    ASSERT_CHANNEL_ID(device, adc_channel);

    ADC_DEBUG_PRINTF(device, "disable channel %d\n", adc_channel);
//    ADC_DelAdcChannel(device->adc.register_base, adc_channel_to_mask(device, adc_channel));
}

//
// ADC conversion API
//

void adc_start_conversion(const adc_device_t *device)
{
//    ASSERT_INITIALIZED(device, STRINGIFY(adc_start_conversion));

    // clear ADC conversion complete flag
    ADC_ClearStatus(device->adc.register_base, ADC_FLAG_EOCA);

    // start ADC conversion
    ADC_Start(device->adc.register_base);
		ADC1_startCov_time = millis();
}

bool adc_is_conversion_completed(const adc_device_t *device)
{
//    ASSERT_INITIALIZED(device, STRINGIFY(adc_is_conversion_completed));

    // check if ADC conversion complete flag is set
    return ADC_GetStatus(device->adc.register_base, ADC_FLAG_EOCA) == SET;
}

void adc_await_conversion_completed(const adc_device_t *device, uint32_t timeOut)
{
//    ASSERT_INITIALIZED(device, STRINGIFY(adc_await_conversion_completed));
    while (!adc_is_conversion_completed(device))
    {
       if(millis() - ADC1_startCov_time >= timeOut)
			 {
				 //log
				 break;
			 }
    }
}

uint16_t adc_conversion_read_result(const adc_device_t *device, const uint8_t adc_channel)
{
//    ASSERT_INITIALIZED(device, STRINGIFY(adc_conversion_read_result));
//    ASSERT_CHANNEL_ID(device, adc_channel);

    // clear ADC conversion complete flag
     ADC_ClearStatus(device->adc.register_base, ADC_FLAG_EOCA);

    // read conversion result directly from DRx register
    uint16_t conversion_results = *(volatile uint16_t *)(&device->adc.register_base->DR0 + adc_channel * 2UL);
    return conversion_results;
}
