/**
 ***************************************************************************************************
 * @file 		i2c_ctrl.c
 * @brief 		I2C Control
 ***************************************************************************************************
 **************************************************************************************************/
#define AD_CTRL_GLOBAL

#include "common.h"
#include <math.h>

#define SAADC_SAMPLES_IN_BUFFER 2
#define SAADC_SAMPLE_RATE 250 /**< SAADC sample rate in ms. */

static const nrf_drv_timer_t   m_timer = NRF_DRV_TIMER_INSTANCE(3);
static nrf_saadc_value_t       m_buffer_pool[2][SAADC_SAMPLES_IN_BUFFER];
static nrf_ppi_channel_t       m_ppi_channel;

/**
 ***************************************************************************************************
 *	@brief			SAADC init function
 *	@details		init SAADC
 **************************************************************************************************/
void timer_handler(nrf_timer_event_t event_type, void* p_context)
{

}

/**
 ***************************************************************************************************
 *	@brief			SAADC init function
 *	@details		init SAADC
 **************************************************************************************************/
void saadc_sampling_event_init(void)
{
    ret_code_t err_code;
    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_timer_config_t timer_config = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_config.frequency = NRF_TIMER_FREQ_31250Hz;
    err_code = nrf_drv_timer_init(&m_timer, &timer_config, timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event */
    uint32_t ticks = nrf_drv_timer_ms_to_ticks(&m_timer, SAADC_SAMPLE_RATE);
    nrf_drv_timer_extended_compare(&m_timer, NRF_TIMER_CC_CHANNEL0, ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
    nrf_drv_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrf_drv_timer_compare_event_address_get(&m_timer, NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_event_addr = nrf_drv_saadc_sample_task_get();

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrf_drv_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_ppi_channel_assign(m_ppi_channel, timer_compare_event_addr, saadc_sample_event_addr);
    APP_ERROR_CHECK(err_code);
}

/**
 ***************************************************************************************************
 *	@brief			SAADC init function
 *	@details		init SAADC
 **************************************************************************************************/
void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrf_drv_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);
}

/**
 ***************************************************************************************************
 *	@brief			SAADC init function
 *	@details		init SAADC
 **************************************************************************************************/
void saadc_callback(nrf_drv_saadc_evt_t const * p_event)
{
    if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
    {
        ret_code_t err_code;
        uint16_t adc_value;
        uint8_t value[SAADC_SAMPLES_IN_BUFFER * 2];
        uint16_t bytes_to_send;

        // set buffers
        err_code = nrf_drv_saadc_buffer_convert(p_event->data.done.p_buffer, SAADC_SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);

        // print samples on hardware UART and parse data for BLE transmission
        for (int i = 0; i < SAADC_SAMPLES_IN_BUFFER; i++)
        {

            adc_value = p_event->data.done.p_buffer[i];
        }

    }
}

/**
 ***************************************************************************************************
 *	@brief			SAADC init function
 *	@details		init SAADC
 **************************************************************************************************/
void saadc_init(void)
{
    ret_code_t err_code;

    nrf_drv_saadc_config_t saadc_config = NRF_DRV_SAADC_DEFAULT_CONFIG;
    saadc_config.resolution = NRF_SAADC_RESOLUTION_12BIT;
    // saadc_config.oversample = NRF_SAADC_OVERSAMPLE_2X;

    nrf_saadc_channel_config_t channel_0_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(LIBAT_TEST_PIN);
    channel_0_config.gain = NRF_SAADC_GAIN1_4;
    channel_0_config.reference = NRF_SAADC_REFERENCE_INTERNAL;

    nrf_saadc_channel_config_t channel_1_config = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(EBICYCLE_BAT_TEST_PIN);
    channel_1_config.gain = NRF_SAADC_GAIN1_5;
    channel_1_config.reference = NRF_SAADC_REFERENCE_INTERNAL;
    // channel_1_config.acq_time = NRF_SAADC_ACQTIME_20US;

    if (false == saadc_on_flag)
    {
        err_code = nrf_drv_saadc_init(&saadc_config, saadc_callback);
        APP_ERROR_CHECK(err_code);

        err_code = nrf_drv_saadc_channel_init(0, &channel_0_config);
        APP_ERROR_CHECK(err_code);
        err_code = nrf_drv_saadc_channel_init(1, &channel_1_config);
        APP_ERROR_CHECK(err_code);

        err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[0], SAADC_SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);
        err_code = nrf_drv_saadc_buffer_convert(m_buffer_pool[1], SAADC_SAMPLES_IN_BUFFER);
        APP_ERROR_CHECK(err_code);

        saadc_on_flag = true;
    }
}


/**
 ***************************************************************************************************
 *	@brief			SAADC stop function
 *	@details		Stop SAADC
 **************************************************************************************************/
void saadc_stop(void)
{
    if (true == saadc_on_flag)
    {
        nrf_drv_saadc_uninit();

        saadc_on_flag = false;
    }
}


/***************************************************************************************************
 **************************************************************************************************/