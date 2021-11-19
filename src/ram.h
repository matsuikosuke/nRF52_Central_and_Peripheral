/**
 ***************************************************************************************************
 * @file        ram.h
 * @brief       global variables
 ***************************************************************************************************
 **************************************************************************************************/
#include "orange_service.h"

#include "define.h"

// BLE Command
extern uint8_t received_write_data_len;
extern uint8_t write_buf[BLE_WRITE_DATA_LENGTH];
extern bool write_flag;
extern bool write_res;
extern uint32_t received_packet_num;
extern uint16_t write_len;
extern uint8_t indication_buf[BLE_ALL_DATA_MAX];
extern uint8_t notification_buf[BLE_ALL_DATA_MAX];
extern uint16_t indication_len;
extern uint8_t indication_packet_num;
extern uint8_t indication_packet_count;
extern uint8_t indication_last_packet_mod;

extern uint8_t ble_cmd;
extern uint8_t ble_indication_unlock_result;
extern uint8_t ble_indication_inspection_result;

extern bool active_mode_flag;
extern bool neglect_check_flag;
extern bool ble_indicate_enable;
extern bool ble_indicate_ack;
extern bool ble_write_flag;
extern bool ble_connect_flag;
extern bool orange_c_ble_connect_flag;

extern uint8_t indication_pattern;
extern uint8_t indication_fail_count;

extern uint16_t mtu_max_size;

extern uint8_t read_buf[10];

extern uint8_t adv_buf[31];
extern uint8_t scan_buf[31];
extern uint8_t beacon_buf[31];

extern uint8_t central_read_buf[10];

// motor control variables
extern uint16_t pwm_width;

// app system timet
extern bool sys_timer_flag[TIMER_NUM];
extern bool sys_timer_limit[TIMER_NUM];
extern uint16_t sys_timer_count[TIMER_NUM];

// buzzer variables
extern uint8_t buzzer_flag;
extern uint8_t freq;
extern uint8_t music_dim;

// UART
extern uint16_t receive_index;
extern uint8_t receive_data[RECEIVE_BUF_MAX];
extern bool uart_on_flag;

// I2C
extern bool m_xfer_done; /* Indicates if operation on TWI has ended. */
extern uint8_t m_sample;
extern uint8_t m8u_index;
extern uint8_t sci6is_index;
extern bool test_flag;
extern bool i2c_on_flag;

// DHT11
extern uint8_t temperature_high;
extern uint8_t temperature_low;
extern uint8_t humidity_high;
extern uint8_t humidity_low;
extern uint8_t parity;

// initialize RAM variables
extern void ram_init(void);

// Device Information
extern uint8_t device_id[DEVICE_ID_LENGTH];

// Front Switch
extern bool front_sw_flag;

// FLASH
extern bool flash_check;
extern bool spi_on_flag;

// AD
extern bool saadc_on_flag;
