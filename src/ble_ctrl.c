/**
 ***************************************************************************************************
 * @file        blr_ctrl.c
 * @brief       BLE Setting
 ***************************************************************************************************
 **************************************************************************************************/
#include "ble.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_db_discovery.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "fds.h"
#include "nordic_common.h"
#include "nrf_fstorage.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "nrf_ble_scan.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "peer_manager.h"
#include "peer_manager_handler.h"
#include "orange_service.h"
#include "orange_service_c.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

// dfu
#include "ble_conn_state.h"
#include "ble_dfu.h"
#include "nrf_bootloader_info.h"
#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_power.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"

#include "common.h"

#define 	OPCODE_LENGTH   1 
#define 	HANDLE_LENGTH   2

/*--------------------------------------------------------------------------------------------------
- BLE Common
--------------------------------------------------------------------------------------------------*/
NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */
NRF_BLE_QWRS_DEF(m_qwr, NRF_SDH_BLE_TOTAL_LINK_COUNT);                      /**< Context for the Queued Write module.*/

#define ORANGE_SERVICE_UUID_IDX      0    /**< Orange service UUID index in array. */

#if defined(WITHOUT_DFU)
static ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
{
    {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN  }
};
#else
static ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
{
    {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN+1  }
};
#endif

/*--------------------------------------------------------------------------------------------------
- BLE Peripheral
--------------------------------------------------------------------------------------------------*/
//BLE_ORANGE_DEF(m_orange); 
static ble_orange_t m_orange;                                                        /**< Orange Service instance. */
BLE_ADVERTISING_DEF(m_advertising);                                 /**< Advertising module instance. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

/*--------------------------------------------------------------------------------------------------
- BLE Central
--------------------------------------------------------------------------------------------------*/
static ble_orange_c_t m_orange_c; 
NRF_BLE_GQ_DEF(m_ble_gatt_queue,                                    
               NRF_SDH_BLE_CENTRAL_LINK_COUNT,
               NRF_BLE_GQ_QUEUE_SIZE);
BLE_DB_DISCOVERY_DEF(m_db_discovery);                         /**< Database discovery module instance. */
NRF_BLE_SCAN_DEF(m_scan);                                           /**< Scanning module instance. */

static uint16_t m_conn_handle_orange_c = BLE_CONN_HANDLE_INVALID; 

static ble_gap_scan_params_t m_scan_param =                 /**< Scan parameters requested for scanning and connection. */
{
    .active        = 0x01,
    .interval      = NRF_BLE_SCAN_SCAN_INTERVAL,
    .window        = NRF_BLE_SCAN_SCAN_WINDOW,
    .filter_policy = BLE_GAP_SCAN_FP_ACCEPT_ALL,
    .timeout       = NRF_BLE_SCAN_SCAN_DURATION,
    .scan_phys     = BLE_GAP_PHY_1MBPS,
    .extended      = true,
};

static char const m_target_periph_name[] = "";

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);
static void scan_start(void);
static void adv_scan_start(void);

/*--------------------------------------------------------------------------------------------------
- BLE Peripheral & BLE Common
--------------------------------------------------------------------------------------------------*/

/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
static bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
    
            break;

        default:
          
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}

// lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);


static void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);

        // Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) = {
    .handler = buttonless_dfu_sdh_state_observer,
};


static void advertising_config_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout = APP_ADV_DURATION;
}


/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

//    gap_conn_params.min_conn_interval = m_scan.conn_params.min_conn_interval;
//    gap_conn_params.max_conn_interval = m_scan.conn_params.max_conn_interval;
//    gap_conn_params.slave_latency     = m_scan.conn_params.slave_latency;
//    gap_conn_params.conn_sup_timeout  = m_scan.conn_params.conn_sup_timeout;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        mtu_max_size = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
    }
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE); // UPDATE MTU HERE
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            NRF_LOG_INFO("Fast advertising.");
            //            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            //            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            //            sleep_mode_enter();
            break;

        default:
            break;
    }
}

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */

static void advertising_init(void)
{
    ret_code_t err_code;
    ble_advertising_init_t init;

#if defined(WITHOUT_DFU)
    ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
    {
        {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN  }
    };
#else
    ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
    {
        {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN+1  }
    };
#endif

    ble_advdata_manuf_data_t manuf_buf;
    manuf_buf.company_identifier = MY_COMPANY_IDENTIFIER;
    manuf_buf.data.p_data = (uint8_t*) &device_id;
    manuf_buf.data.size   = sizeof(device_id);
    
    memset(&init, 0, sizeof(init));

    init.srdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.srdata.p_manuf_specific_data = &manuf_buf;
    init.advdata.include_appearance = false;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids = m_adv_uuids;

    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout = APP_ADV_DURATION;

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

static void disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}

// YOUR_JOB: Update this code if you want to do anything given a DFU event (optional).
/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");

            // Prevent device from advertising on disconnect.
            ble_adv_modes_config_t config;
            advertising_config_get(&config);
            config.ble_adv_on_disconnect_disabled = true;
            ble_advertising_modes_config_set(&m_advertising, &config);

            // Disconnect all other bonded devices that currently are connected.
            // This is required to receive a service changed indication
            // on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
            NRF_LOG_INFO("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}

/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_orange     Instance of LED Button Service to which the write applies.
 * @param[in] led_state Written/desired state of the LED.
 */
static void orange_write_handler(uint16_t conn_handle, ble_orange_t * p_orange, const uint8_t * receive_buf, uint8_t length)
{
    uint8_t i;

    for (i = 0; i < length; i++)
    {
        write_buf[i] = receive_buf[i];
    }
    write_len = length;
}

/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    ret_code_t err_code;
    ble_orange_init_t orange_init;
    nrf_ble_qwr_init_t qwr_init = { 0 };
    ble_dfu_buttonless_init_t dfus_init = { 0 };

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    for (uint32_t i = 0; i < NRF_SDH_BLE_TOTAL_LINK_COUNT; i++)
    {
        err_code = nrf_ble_qwr_init(&m_qwr[i], &qwr_init);
        APP_ERROR_CHECK(err_code);
    }

#if defined(WITHOUT_DFU)
#else
    // Initialize DFU.
    dfus_init.evt_handler = ble_dfu_evt_handler;

    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);
#endif

    // Initialize the Test Service.
    memset(&orange_init, 0, sizeof(orange_init));
    orange_init.orange_write_handler = orange_write_handler;

    err_code = ble_orange_init(&m_orange, &orange_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module that
 *          are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply
 *       setting the disconnect_on_fail config parameter, but instead we use the event
 *       handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ret_code_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing power management.
 */
void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for initializing the advertising and the scanning.
 */
static void adv_scan_start(void)
{
    ret_code_t err_code;

    //check if there are no flash operations in progress
    if (!nrf_fstorage_is_busy(NULL))
    {
        // Start scanning for peripherals and initiate connection to devices which
        // advertise Heart Rate or Running speed and cadence UUIDs.
#if defined(WITHOUT_SCAN)
#else
    // Initialize the async SVCI interface to bootloader before any interrupts are enabled.
    scan_start();
#endif        

        // Start advertising.
        err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);
    }
}

/**@brief Function for starting advertising.
 */
void advertising_start(void)
{
    ret_code_t err_code;
    err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}

//advertising_stop
static void advertising_stop(void)
{
    ret_code_t err_code;
    err_code=sd_ble_gap_adv_stop(m_advertising.adv_handle);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    //pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);

    switch (p_evt->evt_id)
    {
        case PM_EVT_PEERS_DELETE_SUCCEEDED:
            adv_scan_start();
            break;

        default:
            break;
    }
}

/**@brief Function for assigning new connection handle to available instance of QWR module.
 *
 * @param[in] conn_handle New connection handle.
 */
static void multi_qwr_conn_handle_assign(uint16_t conn_handle)
{
    for (uint32_t i = 0; i < NRF_BLE_LINK_COUNT; i++)
    {
        if (m_qwr[i].conn_handle == BLE_CONN_HANDLE_INVALID)
        {
            ret_code_t err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr[i], conn_handle);
            APP_ERROR_CHECK(err_code);
            break;
        }
    }
}

/**@brief   Function for handling BLE events from peripheral applications.
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_peripheral_evt(ble_evt_t const * p_ble_evt)
{
    ret_code_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected.");
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            multi_qwr_conn_handle_assign(m_conn_handle);
            ble_connect_flag = true;

            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected, reason %d.", p_ble_evt->evt.gap_evt.params.disconnected.reason);
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            ble_connect_flag = false;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys = {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        }
        break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            NRF_LOG_DEBUG("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
            break;

        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_AUTH_KEY_REQUEST");
            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_LESC_DHKEY_REQUEST");
            break;

         case BLE_GAP_EVT_AUTH_STATUS:
             NRF_LOG_INFO("BLE_GAP_EVT_AUTH_STATUS: status=0x%x bond=0x%x lv4: %d kdist_own:0x%x kdist_peer:0x%x",
                          p_ble_evt->evt.gap_evt.params.auth_status.auth_status,
                          p_ble_evt->evt.gap_evt.params.auth_status.bonded,
                          p_ble_evt->evt.gap_evt.params.auth_status.sm1_levels.lv4,
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_own),
                          *((uint8_t *)&p_ble_evt->evt.gap_evt.params.auth_status.kdist_peer));
            break;

        case BLE_GATTS_EVT_HVC:
            // ble_indicate_ack = true;
            break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}

void indication_exe(uint8_t * data, uint8_t len)
{
    ble_orange_indication(&m_orange, data, len);
}

void notification_exe(uint8_t * data, uint8_t len)
{
    ble_orange_notification(&m_orange, data, len);
}

static void data_len_set(uint8_t value)
{
    ret_code_t err_code;
    err_code = nrf_ble_gatt_data_length_set(&m_gatt, BLE_CONN_HANDLE_INVALID, value);
    APP_ERROR_CHECK(err_code);
}



/**
 ***************************************************************************************************
 *	@brief			BLE Disconnect要求処理
 *	@details		BLE Disconnect要求する
 *
 *	@return			None
 *
 *	@par			Note
 *					 - Disconnect完了はEventで取得すること
 *	@par			History
 **************************************************************************************************/
void ble_req_disconnect(void)
{
    uint32_t err;
    err = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    APP_ERROR_CHECK(err);
}


/*--------------------------------------------------------------------------------------------------
- BLE Central
--------------------------------------------------------------------------------------------------*/
static void scan_evt_handler(scan_evt_t const * p_scan_evt)
{
    ret_code_t err_code;
    ble_gap_evt_adv_report_t const * p_adv = 
                   p_scan_evt->params.filter_match.p_adv_report;
    ble_gap_scan_params_t    const * p_scan_param = 
                   p_scan_evt->p_scan_params;

    switch(p_scan_evt->scan_evt_id)
    {
        case NRF_BLE_SCAN_EVT_FILTER_MATCH:
        {
            // Initiate connection.
            err_code = sd_ble_gap_connect(&p_adv->peer_addr,
                                          p_scan_param,
                                          &m_scan.conn_params,
                                          APP_BLE_CONN_CFG_TAG);
            APP_ERROR_CHECK(err_code);
        } break;

        default:
            break;
    }
}

/**@brief Function for initialization the scanning and setting the filters.
 */
static void scan_init(void)
{
    ret_code_t          err_code;
    nrf_ble_scan_init_t init_scan;
    
#if defined(WITHOUT_DFU)
    ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
    {
        {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN  }
    };
#else
    ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
    {
        {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN+1  }
    };
#endif

    memset(&init_scan, 0, sizeof(init_scan));

    init_scan.p_scan_param = &m_scan_param;

    err_code = nrf_ble_scan_init(&m_scan, &init_scan, scan_evt_handler);
    APP_ERROR_CHECK(err_code);

    if (strlen(m_target_periph_name) != 0)
    {
        err_code = nrf_ble_scan_filter_set(&m_scan, 
                                           SCAN_NAME_FILTER, 
                                           m_target_periph_name);
        APP_ERROR_CHECK(err_code);
    }

    err_code = nrf_ble_scan_filter_set(&m_scan, 
                                       SCAN_UUID_FILTER, 
                                       &m_adv_uuids[ORANGE_SERVICE_UUID_IDX]);
    APP_ERROR_CHECK(err_code);


    err_code = nrf_ble_scan_filters_enable(&m_scan, 
                                           NRF_BLE_SCAN_ALL_FILTER, 
                                           false);
    APP_ERROR_CHECK(err_code);

}

/**@brief Function for changing filter settings after establishing the connection.
 */
static void filter_settings_change(void)
{
    ret_code_t err_code;
    
#if defined(WITHOUT_DFU)
    ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
    {
        {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN  }
    };
#else
    ble_uuid_t m_adv_uuids[] =                                   /**< Universally unique service identifiers. */
    {
        {ORANGE_UUID_SERVICE, BLE_UUID_TYPE_VENDOR_BEGIN+1  }
    };
#endif

    err_code = nrf_ble_scan_all_filter_remove(&m_scan);
    APP_ERROR_CHECK(err_code);

    if (strlen(m_target_periph_name) != 0)
    {
        err_code = nrf_ble_scan_filter_set(&m_scan, 
                                           SCAN_NAME_FILTER, 
                                           m_target_periph_name);
        APP_ERROR_CHECK(err_code);
    }

    if (m_conn_handle_orange_c != BLE_CONN_HANDLE_INVALID)
    {
        err_code = nrf_ble_scan_filter_set(&m_scan, 
                                           SCAN_UUID_FILTER, 
                                           &m_adv_uuids[ORANGE_SERVICE_UUID_IDX]);
    }

    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the scanning.
 */
static void scan_start(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_scan_start(&m_scan);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("Scanning");
}

/**@brief Function for stopping the scanning.
 */
static void scan_stop(void)
{
    nrf_ble_scan_stop();

    NRF_LOG_INFO("Stop Scan");
}

/**@brief Function for handling database discovery events.
 *
 * @details This function is a callback function to handle events from the database discovery module.
 *          Depending on the UUIDs that are discovered, this function forwards the events
 *          to their respective services.
 *
 * @param[in] p_event  Pointer to the database discovery event.
 */
static void db_disc_handler(ble_db_discovery_evt_t * p_evt)
{
    ble_db_discovery_t const * p_db = (ble_db_discovery_t *)p_evt->params.p_db_instance;

    ble_orange_on_db_disc_evt(&m_orange_c, p_evt);

    if (p_evt->evt_type == BLE_DB_DISCOVERY_AVAILABLE) {
        NRF_LOG_INFO("DB Discovery instance %p available on conn handle: %d",
                     p_db,
                     p_evt->conn_handle);
        NRF_LOG_INFO("Found %d services on conn_handle: %d",
                     p_db->srv_count,
                     p_evt->conn_handle);
    }
}

/**
 * @brief Database discovery initialization.
 */
static void db_discovery_init(void)
{
    ble_db_discovery_init_t db_init;

    memset(&db_init, 0, sizeof(db_init));

    db_init.evt_handler =  db_disc_handler;
    db_init.p_gatt_queue = &m_ble_gatt_queue;

    ret_code_t err_code = ble_db_discovery_init(&db_init);
    APP_ERROR_CHECK(err_code);
}

/**@brief   Function for handling BLE events from the central application.
 *
 * @details This function parses scanning reports and initiates a connection to peripherals when a
 *          target UUID is found. 
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 */
static void on_ble_central_evt(ble_evt_t const * p_ble_evt)
{
    ret_code_t            err_code;
    ble_gap_evt_t const * p_gap_evt = &p_ble_evt->evt.gap_evt;
    ble_gap_evt_adv_report_t const * p_adv_report = &p_ble_evt->evt.gap_evt.params.adv_report;
    

    switch (p_ble_evt->header.evt_id)
    {
        // Upon connection, check which peripheral is connected (HR or RSC), initiate DB
        // discovery, update LEDs status, and resume scanning, if necessary.
        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_INFO("Central connected");
            // If no Smartlock is currently connected, try to find them on this peripheral.
            if (m_conn_handle_orange_c == BLE_CONN_HANDLE_INVALID)
            {
                NRF_LOG_INFO("CENTRAL: Searching for Orange on conn_handle 0x%x", p_gap_evt->conn_handle);

                err_code = ble_db_discovery_start(&m_db_discovery, p_gap_evt->conn_handle);
                APP_ERROR_CHECK(err_code);
            }

            // Assign connection handle to the QWR module.
            multi_qwr_conn_handle_assign(p_gap_evt->conn_handle);

            orange_c_ble_connect_flag = true;
        } break; // BLE_GAP_EVT_CONNECTED

        // Upon disconnection, reset the connection handle of the peer that disconnected.
        case BLE_GAP_EVT_DISCONNECTED:
        {
            if (p_gap_evt->conn_handle == m_conn_handle_orange_c)
            {
                NRF_LOG_INFO("Orange central disconnected (reason: %d)",
                             p_gap_evt->params.disconnected.reason);

                m_conn_handle_orange_c = BLE_CONN_HANDLE_INVALID;
                
//                filter_settings_change();
            }

            if (m_conn_handle_orange_c == BLE_CONN_HANDLE_INVALID)
            {
                // Start scanning.
                scan_start();
            }

            orange_c_ble_connect_flag = false;
        } break; // BLE_GAP_EVT_DISCONNECTED

        case BLE_GAP_EVT_TIMEOUT:
        {
            // No timeout for scanning is specified, so only connection attemps can timeout.
            if (p_gap_evt->params.timeout.src == BLE_GAP_TIMEOUT_SRC_CONN)
            {
                NRF_LOG_INFO("Connection Request timed out.");
            }
        } break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
        {
            // Accept parameters requested by peer.
            err_code = sd_ble_gap_conn_param_update(p_gap_evt->conn_handle,
                                        &p_gap_evt->params.conn_param_update_request.conn_params);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;
            
        case BLE_GAP_EVT_ADV_REPORT:
        {
                uint16_t length;
                
                length = p_adv_report->data.len;
                for (int i=0; i<length; i++)
                {
                    if((1 == p_adv_report->type.connectable 
                        || 1 == p_adv_report->type.directed
                        || 1 == p_adv_report->type.scannable
                        || 1 == p_adv_report->type.extended_pdu)
                        && 0 == p_adv_report->type.scan_response)
                    {
                        adv_buf[i] = p_adv_report->data.p_data[i];
                    }
                    else if(1 == p_adv_report->type.scan_response)
                    {
                        scan_buf[i] = p_adv_report->data.p_data[i];
                    }
                }
                //if ((p_adv_report->dlen == 30 ) && (ble_state == BLE_CONNECTED) &&
                    //(p_adv_report->data[25] == 0x34) &&
                    //(p_adv_report->data[26] == 0x12) &&
                    //(p_adv_report->data[27] == 0x78) &&
                    //(p_adv_report->data[2] == 0x1a))
                    //ble_nus_string_send(&m_nus, p_adv_report->data, p_adv_report->dlen);
        }
        break;

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Function for handling the Heart Rate Service Client
 *        Running Speed and Cadence Service Client.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void service_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Handles events coming from the Orange central module.
 */
static void orange_c_evt_handler(ble_orange_c_t * p_orange_c, ble_orange_c_evt_t * p_orange_c_evt)
{
    switch (p_orange_c_evt->evt_type)
    {
        case BLE_ORANGE_C_EVT_DISCOVERY_COMPLETE:
        {
            if (m_conn_handle_orange_c == BLE_CONN_HANDLE_INVALID)
            {
                ret_code_t err_code;

                m_conn_handle_orange_c = p_orange_c_evt->conn_handle;
                NRF_LOG_INFO("Orange discovered on conn_handle 0x%x", m_conn_handle_orange_c );

                filter_settings_change();

                err_code = ble_orange_c_handles_assign(p_orange_c,
                                                    m_conn_handle_orange_c ,
                                                    &p_orange_c_evt->params.peer_db);
                APP_ERROR_CHECK(err_code);

#if defined(BUG_AND_TEMPORARY_STOP)
#else
                // Initiate bonding.
                err_code = pm_conn_secure(m_conn_handle_orange_c , false);
                if (err_code != NRF_ERROR_BUSY)
                {
                    APP_ERROR_CHECK(err_code);
                }
#endif


                // Orange service discovered. Enable notification.
                err_code = ble_orange_c_orange_notification_enable(p_orange_c);
                APP_ERROR_CHECK(err_code);

                // Orange service discovered. Enable indication.
                err_code = ble_orange_c_orange_indication_enable(p_orange_c);
                APP_ERROR_CHECK(err_code);

            }
        } break; // BLE_ORANGE_C_EVT_DISCOVERY_COMPLETE

        case BLE_ORANGE_C_EVT_NOTIFICATION:
        {
        } break; // BLE_ORANGE_C_EVT_NOTIFICATION

        case BLE_ORANGE_C_EVT_INDICATION:
        {
        } break; // BLE_ORANGE_C_EVT_INDICATION

        default:
            // No implementation needed.
            break;
    }
}

/**@brief Orange initialization.
 */
static void orange_c_init(void)
{
    ret_code_t       err_code;
    ble_orange_c_init_t orange_c_init_obj;

    orange_c_init_obj.evt_handler   = orange_c_evt_handler;
    orange_c_init_obj.error_handler = service_error_handler;
    orange_c_init_obj.p_gatt_queue  = &m_ble_gatt_queue;

    err_code = ble_orange_c_init(&m_orange_c, &orange_c_init_obj);
    APP_ERROR_CHECK(err_code);
}

/*--------------------------------------------------------------------------------------------------
- BLE Common
--------------------------------------------------------------------------------------------------*/
/**@brief Function for checking whether a bluetooth stack event is an advertising timeout.
 *
 * @param[in] p_ble_evt Bluetooth stack event.
 */
static bool ble_evt_is_advertising_timeout(ble_evt_t const * p_ble_evt)
{
    return (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_SET_TERMINATED);
}

static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint16_t conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
    uint16_t role        = ble_conn_state_role(conn_handle);

    // Based on the role this device plays in the connection, dispatch to the right handler.
    if (role == BLE_GAP_ROLE_PERIPH || ble_evt_is_advertising_timeout(p_ble_evt))
    {
        ble_orange_on_ble_evt(p_ble_evt, &m_orange);
        on_ble_peripheral_evt(p_ble_evt);
    }
    else if ((role == BLE_GAP_ROLE_CENTRAL) || (p_ble_evt->header.evt_id == BLE_GAP_EVT_ADV_REPORT))
    {
        ble_orange_c_on_ble_evt(p_ble_evt, &m_orange_c);
        on_ble_central_evt(p_ble_evt);
    }
}

void ble_init(void)
{
    ret_code_t err_code;

    // NRF_POWER->DCDCEN = 1;
    ble_stack_init();
    scan_init();

    // enable DC/DC mode
    sd_power_dcdc_mode_set(NRF_POWER_DCDC_ENABLE);
    //
    //    //NRF_RADIO->TASKS_TXEN = 1;
    //    NRF_RADIO->TASKS_RXEN = 1;
    //    NRF_RADIO->TASKS_START = 1;

    gap_params_init();
    gatt_init();
    conn_params_init();
    db_discovery_init();
    peer_manager_init();
    orange_c_init();
    services_init();
    advertising_init();
    data_len_set(DATA_LENGTH_MAX);

    err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_SCAN_INIT, m_conn_handle, 4);
    APP_ERROR_CHECK(err_code);

    adv_scan_start();
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/ 
static uint8_t write_test_cyc = 0;
static uint8_t send_data[10] = "0123456789";
void central_write_notification_test(void)
{
    //uint8_t write_size = 10;//sizeof(data);

    if(true == sys_timer_limit[BLE_TEST_TIMER] && true == orange_c_ble_connect_flag)
    {
        sys_timer_limit[BLE_TEST_TIMER] = false;
        write_res = false;

        for(int i=0; i<10; i++)
        {
            send_data[i] = '0'+ write_test_cyc%10;
        }

        ble_orange_data_write(&m_orange_c, &send_data[0], 10);
//        ble_orange_status_send(&m_orange_c, send_data[0]);

        write_test_cyc += 1;
    }
}