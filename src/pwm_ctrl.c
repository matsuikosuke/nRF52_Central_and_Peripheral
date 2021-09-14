/**
 ***************************************************************************************************
 * @file    pwm_ctrl.c
 * @brief   PWM Control
 ***************************************************************************************************
 **************************************************************************************************/
#define PWM_GLOBAL
#include "common.h"
#define const_value (freq / 2)

static nrf_drv_pwm_t m_pwm0 = NRF_DRV_PWM_INSTANCE(0);
static nrf_drv_pwm_t m_pwm1 = NRF_DRV_PWM_INSTANCE(1);

static nrf_pwm_values_individual_t m_seq_values;
static nrf_pwm_sequence_t const    m_seq =
{
    .values.p_individual = &m_seq_values,
    .length              = NRF_PWM_VALUES_LENGTH(m_seq_values),
    .repeats             = 0,
    .end_delay           = 0
};

static uint16_t scale_value;


static void pwm0_handler(nrf_drv_pwm_evt_type_t event_type);

/**
 ***************************************************************************************************
 *	@brief			PWM0 Sart function
 *	@details		Start PWM0
 **************************************************************************************************/
void pwm0_start(uint16_t duty, bool dir)
{
    pwm0_stop();
    nrf_drv_pwm_config_t const pwm0_config =
    {
        .output_pins =
        {
            PWM0_INA_PIN, // channel 0
            PWM0_INB_PIN, // channel 1
            NRF_DRV_PWM_PIN_NOT_USED, // channel 2
            NRF_DRV_PWM_PIN_NOT_USED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_125kHz,  //1cyc 8usec
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 12500, // 100ms period(125khz clk)
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm0, &pwm0_config, pwm0_handler));

    uint16_t front_duty = 0x0000;
    uint16_t back_duty = 0x0000;

    if (dir)
        front_duty = duty;
    else
        back_duty = duty;

    m_seq_values.channel_0 = front_duty;
    m_seq_values.channel_1 = back_duty;
    m_seq_values.channel_2 = 0;
    m_seq_values.channel_3 = 0;

    (void)nrf_drv_pwm_simple_playback(&m_pwm0, &m_seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

/**
 ***************************************************************************************************
 *	@brief			PWM0 Handler
 *	@details		PWM0 Handler
 **************************************************************************************************/
static void pwm0_handler(nrf_drv_pwm_evt_type_t event_type)
{
    if (event_type == NRF_DRV_PWM_EVT_FINISHED)
    {
        uint16_t * p_channels = (uint16_t *)&m_seq_values;
        uint16_t value = p_channels[0];

        value = pwm_width;
    }
}

/**
 ***************************************************************************************************
 *	@brief			PWM0 Stop function
 *	@details		Stop PWM0
 **************************************************************************************************/
void pwm0_stop(void)
{
    if (&m_pwm0 != NULL)
    {
        nrf_drv_pwm_uninit(&m_pwm0);
    }
}

/**
 ***************************************************************************************************
 *	@brief			PWM1 Sart function
 *	@details		Start PWM1
 **************************************************************************************************/
void pwm1_start(uint16_t freq)
{
    pwm1_stop();
    nrf_drv_pwm_config_t const pwm1_config =
    {
        .output_pins =
        {
            DRV_BUZ_PIN, // channel 0
            NRF_DRV_PWM_PIN_NOT_USED, // channel 1
            NRF_DRV_PWM_PIN_NOT_USED, // channel 2
            NRF_DRV_PWM_PIN_NOT_USED  // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_125kHz,  //1cyc 1usec
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = freq,//music_bigclock_data[music_dim][0],//600, // 400us period(1MHz clk)
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrf_drv_pwm_init(&m_pwm1, &pwm1_config, NULL));

    scale_value = freq/2;
    nrf_pwm_sequence_t const seq =
    {
        .values.p_common = &scale_value,
        .length          = NRF_PWM_VALUES_LENGTH(scale_value),
        .repeats         = 0,
        .end_delay       = 4
    };

    (void)nrf_drv_pwm_simple_playback(&m_pwm1, &seq, 1, NRF_DRV_PWM_FLAG_LOOP);
}

/**
 ***************************************************************************************************
 *	@brief			PWM1 Stop function
 *	@details		Stop PWM1
 **************************************************************************************************/
void pwm1_stop(void)
{
    if (&m_pwm1 != NULL)
    {
        nrf_drv_pwm_uninit(&m_pwm1);
    }
}