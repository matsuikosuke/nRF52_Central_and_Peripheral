/**
 ***************************************************************************************************
 * @file        ram.c
 * @brief       global variables
 ***************************************************************************************************
 **************************************************************************************************/
#include "common.h"
#include "orange_service.h"
#include <stdint.h>
#include <string.h>

// BLE Command
uint8_t received_write_data_len;
uint8_t write_buf[BLE_WRITE_DATA_LENGTH] = { 0 };
bool write_flag = false;
bool write_res = false;
uint8_t reassembly_write_buf[BLE_ALL_DATA_MAX] = { 0 };
uint32_t received_packet_num;
uint16_t next_reassembly_write_index;
uint16_t write_len;
uint8_t indication_buf[BLE_ALL_DATA_MAX] = { 0 };
uint8_t notification_buf[BLE_ALL_DATA_MAX] = { 0 };
uint8_t decrypted_write_buf[BLE_INDICATION_DATA_LENGTH] = { 0 };
uint16_t indication_len;
uint8_t indication_packet_num;
uint8_t indication_packet_count;
uint8_t indication_last_packet_mod;

uint8_t ble_cmd;
uint8_t ble_indication_unlock_result;
uint8_t ble_indication_inspection_result;

bool active_mode_flag;
bool neglect_check_flag;
bool ble_indicate_flag;
bool ble_indicate_enable;
bool ble_indicate_ack;
bool ble_write_flag;
bool ble_connect_flag;
bool orange_c_ble_connect_flag = false;

uint8_t indication_pattern;
uint8_t indication_fail_count;

uint16_t mtu_max_size;

uint8_t read_buf[10] = "0123456789";

uint8_t adv_buf[31] = { 0 };
uint8_t scan_buf[31] = { 0 };

uint8_t central_read_buf[10] ={ 0 };

// ble_orange_t stoptexDoorService;

// motor control variables
uint16_t pwm_width;
bool motor_direction;
uint8_t motor_flag;
bool lock_read_enable;
uint8_t lock_pos_flag;
bool open_command_enable;
bool expected_position_sensor;
bool motor_init_enable;

// app system timer
bool sys_timer_flag[TIMER_NUM];
bool sys_timer_limit[TIMER_NUM];
uint16_t sys_timer_count[TIMER_NUM];
uint16_t active_mode_count_num;
uint8_t ec_count_num;
uint8_t sub_low_power_count_num;
uint8_t sub_reset_count_num;
uint8_t sub_ble_connect_check_count_num;

// buzzer variables
uint8_t buzzer_flag = MELODY_STOP;
uint8_t freq;
uint8_t music_dim;

// UART
uint16_t receive_index;
uint8_t receive_data[RECEIVE_BUF_MAX];
// uint8_t message_data[RECEIVE_BUF_MAX];
uint8_t ec_protcol_num;
uint8_t ec_process_num;
uint8_t ec_check_num;
bool uart_on_flag = false;

// I2C
bool m_xfer_done = false; /* Indicates if operation on TWI has ended. */
uint8_t m_sample;
uint8_t m8u_index;
uint8_t sci6is_index;
bool test_flag;
bool i2c_on_flag = false;

// Device Information
uint8_t device_id[DEVICE_ID_LENGTH];
uint8_t imsi_buf[IMSI_LENGTH];
uint8_t softdevice_version_buf[SOFTDEVICE_VERSION_LENGTH] = SOFTDEVICE_VERSION;


// Front Switch
bool front_sw_flag;

// FLASH
bool flash_check = false;
bool spi_on_flag = false;

// AD
bool saadc_on_flag;


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_orange       ORANGE Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static uint8_t id_to_string(uint8_t num)
{
    uint8_t trans_num;

    if (num >= 0x00 && num < 0x0A)
    {
        trans_num = num + '0';
    }
    else if (num >= 0x0A && num < 0x10)
    {
        trans_num = num - 0x0A + 'A';
    }
    else
    {
        trans_num = '0';
    }

    return trans_num;
}

// initialize RAM variables
void ram_init(void)
{
    // Device Information
    uint32_t temp_device_id = NRF_FICR->DEVICEID[0];
    device_id[0] = id_to_string((uint8_t)((0xF0000000 & temp_device_id) >> 28));
    device_id[1] = id_to_string((uint8_t)((0x0F000000 & temp_device_id) >> 24));
    device_id[2] = id_to_string((uint8_t)((0x00F00000 & temp_device_id) >> 20));
    device_id[3] = id_to_string((uint8_t)((0x000F0000 & temp_device_id) >> 16));
    device_id[4] = id_to_string((uint8_t)((0x0000F000 & temp_device_id) >> 12));
    device_id[5] = id_to_string((uint8_t)((0x00000F00 & temp_device_id) >> 8));
    device_id[6] = id_to_string((uint8_t)((0x000000F0 & temp_device_id) >> 4));
    device_id[7] = id_to_string((uint8_t)(0x0000000F & temp_device_id));

}