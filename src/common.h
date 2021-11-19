/**
 ***************************************************************************************************
 * @file        common.h
 * @brief       ヘッダーファイル
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef COMMON_H /* COMMON_H */
#define COMMON_H

#include <stdbool.h>

#include "define.h"
#include "if.h"
#include "ram.h"

// app
#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"

// nrf drivers
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_gpiote.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_pwm.h"
#include "nrf_drv_rng.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_spi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_twi.h"

// modules
#include "ad_ctrl.h"
#include "ble_ctrl.h"
#include "buzzer_ctrl.h"
#include "cpu_set.h"
#include "i2c_ctrl.h"
#include "lis3dh_ctrl.h"
#include "pwm_ctrl.h"
#include "spi_ctrl.h"
#include "timer.h"
#include "uart_ctrl.h"
#include "w25q80dv_ctrl.h"
#include "dht11_ctrl.h"
#include "sht31_ctrl.h"

// dfu
#include "ble_dfu.h"

#endif /* COMMON_H */
/***************************************************************************************************
 **************************************************************************************************/