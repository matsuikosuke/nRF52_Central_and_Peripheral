/**
 ***************************************************************************************************
 * @file    cpu_set.c
 * @brief   CPU Setting
 ***************************************************************************************************
 **************************************************************************************************/
#define CPU_SET_GLOBAL
#include "common.h"

static void gpio_init(void);
static void wdt_init(void);

void front_sw_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    front_sw_flag = true;
}

/**
 ***************************************************************************************************
 *	@brief			CPU Modules initialization function
 *	@details		Initialize CPU Modules
 **************************************************************************************************/
void cpu_init(void)
{
    // Port initialize setting
    gpio_init();

    // UART initialize setting
    //uart_init_ec();

    // Timer initialize setting
    timers_init();

    // I2C initialize setting
    //twi_init();

    // WDT initialize setting
    wdt_init();

    // AD initialize setting
    // saadc_sampling_event_init();
//    saadc_init();
    // saadc_sampling_event_enable();

    // RAM initialize setting
    ram_init();
}

/**
 * @brief Function for configuring: PIN_IN pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_out_config_t out_config = GPIOTE_CONFIG_OUT_SIMPLE(false);

    nrf_drv_gpiote_in_config_t front_sw_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    front_sw_config.pull = NRF_GPIO_PIN_NOPULL;


    //-----------------------------------------------
    // Front switch
    err_code = nrf_drv_gpiote_in_init(FRONT_SW_PIN, &front_sw_config, front_sw_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(FRONT_SW_PIN, true);

    //-----------------------------------------------
    // AD enable
    nrf_gpio_cfg_output(TESTBAT_TRIGER_PIN);
    nrf_gpio_pin_clear(TESTBAT_TRIGER_PIN); // default:Low


    //-----------------------------------------------
    // UART
    nrf_gpio_cfg_output(TXD_PIN);
    nrf_gpio_pin_set(TXD_PIN); // default:High

    nrf_gpio_cfg_input(RXD_PIN, NRF_GPIO_PIN_NOPULL);


    //-----------------------------------------------
    // OPEN GPIO

    //-----------------------------------------------
    // SPI CS
    nrf_gpio_cfg_output(F_NSS_PIN);
    nrf_gpio_pin_set(F_NSS_PIN); // default:High
    
    //-----------------------------------------------
    // DHT11 BUS
    nrf_gpio_cfg_output(DHT11_BUS_PIN);
    nrf_gpio_pin_set(DHT11_BUS_PIN); // default:High

    //nrf_gpio_cfg_input(DHT11_BUS_PIN, NRF_GPIO_PIN_PULLUP);
}

/**
***************************************************************************************************
*	@brief			WDT initialization function
*	@details		Initialize WDT Module
**************************************************************************************************/
static void wdt_init(void)
{
    NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | (WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV = 3 * 32768;          // 3sec
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk; // Enable reload register 0
    NRF_WDT->TASKS_START = 1;
}
