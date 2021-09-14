/**
 ***************************************************************************************************
 * @file        orange_service.c
 * @brief       Set Orange Service
 ***************************************************************************************************
 **************************************************************************************************/
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_ORANGE)
#include "orange_service.h"
#include "ble_srv_common.h"
#include  "common.h"

static void ble_indication_end(void);
static void ble_disconnect_timer_reset(void);

static void ble_check_device_info(void);
static void ble_check_inspection(void);
static void ble_check_operation(void);


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_orange       ORANGE Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_orange_t * p_orange, ble_evt_t const * p_ble_evt)
{
    p_orange->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_orange       SmartLock Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_orange_t * p_orange, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_orange->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**
 ***************************************************************************************************
 *	@brief			on read function
 *	@details		Function for handling the Read event.
 *      @param[in] p_orange  Orange Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 *      @param[in] data         Response data.
 **************************************************************************************************/
static void on_read(ble_orange_t * p_orange, ble_evt_t const * p_ble_evt, uint8_t* data, uint8_t data_size)
{
    uint32_t err_code;

    ble_gatts_rw_authorize_reply_params_t reply_params;

    memset(&reply_params, 0, sizeof(reply_params));
      
    reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
    reply_params.params.read.p_data    = data;
    reply_params.params.read.len       = data_size;//sizeof(&data);//
    reply_params.params.read.offset    = 0;
    reply_params.params.read.update    = 1;
    reply_params.params.read.gatt_status = NRF_SUCCESS;
      
    sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gap_evt.conn_handle, &reply_params);
}

/**
 ***************************************************************************************************
 *	@brief			on write function
 *	@details		Function for handling the Write event.
 *      @param[in] p_orange  Orange Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
static void on_write(ble_orange_t * p_orange, ble_evt_t const * p_ble_evt)
{
    bool result;
    bool analysis_result;
    uint32_t err_code;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
    if ((p_evt_write->handle == p_orange->command_trans_handles.value_handle)
        && (p_evt_write->len >= 1)
        && (p_orange->orange_write_handler != NULL))
    {
        p_orange->orange_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_orange, &p_evt_write->data[0], p_evt_write->len);

        received_write_data_len = p_evt_write->len;
        write_flag = true;

        // Store received data in write buffer
        for(int i=0; i<p_evt_write->len; i++)
        {
            write_buf[i] = p_evt_write->data[i];
        }
    }
}

/**
 ***************************************************************************************************
 *	@brief			on write function
 *	@details		Function for handling the Write event.
 *      @param[in] p_orange  Orange Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
static void on_write_res(ble_orange_t * p_orange, ble_evt_t const * p_ble_evt)
{
    bool result;
    bool analysis_result;
    uint32_t err_code;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;

    uint8_t handle=p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle;
    ble_gatts_rw_authorize_reply_params_t auth_reply;
    memset(&auth_reply, 0, sizeof(auth_reply));
    auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
    auth_reply.params.write.offset    = 0;
    auth_reply.params.write.update    = 1;

    if(handle == p_orange->command_trans_handles.value_handle
        && (p_evt_write->len >= 1)
        && (p_orange->orange_write_handler != NULL))
    {
        auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
        err_code=sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,&auth_reply);
        //err_code=sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gap_evt.conn_handle, &auth_reply);
        APP_ERROR_CHECK(err_code);

        p_orange->orange_write_handler(p_ble_evt->evt.gatts_evt.conn_handle, p_orange, &p_evt_write->data[0], p_evt_write->len);
        received_write_data_len = p_evt_write->len;
        write_flag = true;

        // Store received data in write buffer
        for(int i=0; i<p_evt_write->len; i++)
        {
            write_buf[i] = p_evt_write->data[i];
        }
    }
}

/**
 ***************************************************************************************************
 *	@brief			on BLE EVENT function
 *	@details		Function for handling the BLE event.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
void ble_orange_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;
    ble_orange_t * p_orange = (ble_orange_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_orange, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_orange, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_orange, p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            if ( p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ  )
            {
                if (p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle == p_orange->orange_read_handles.value_handle)
                {
                    on_read(p_orange, p_ble_evt, &read_buf[0], 10);
                }
            }
            else if(p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
            {
                on_write_res(p_orange, p_ble_evt);
            }
            break;

        case BLE_GATTS_EVT_HVC:
            ble_indicate_ack = true;
            indication_packet_count += 1;
            ble_indicate_enable = true;
            break;


        default:
            // No implementation needed.
            break;
    }
}

/**
 ***************************************************************************************************
 *	@brief			LockState Characteristic Setting
 *	@details		Setting LockState Characteristic
 *      @param[in] p_orange  Orange Service structure.
 **************************************************************************************************/
static uint32_t orange_read_char_add(ble_orange_t * p_orange, const ble_orange_init_t * p_orange_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify   = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_orange->uuid_type;
    ble_uuid.uuid = ORANGE_READ_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    //attr_md.read_perm.sm = 1;      // Security Mode 1 Level 1: Open link
    //attr_md.read_perm.lv = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_READ_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_orange->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_orange->orange_read_handles);
}

/**
 ***************************************************************************************************
 *	@brief			Write Characteristic Setting
 *	@details		Setting Write Characteristic
 *      @param[in] p_orange  Orange Service structure.
 **************************************************************************************************/
static uint32_t command_write_char_add(ble_orange_t * p_orange, const ble_orange_init_t * p_orange_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_orange->uuid_type;
    ble_uuid.uuid = ORANGE_WRITE_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 1;  //Write with Response
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_WRITE_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_orange->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_orange->command_trans_handles);
}

/**
 ***************************************************************************************************
 *	@brief			Indication Characteristic Setting
 *	@details		Setting Indication Characteristic
 *      @param[in] p_orange  Orange Service structure.
 **************************************************************************************************/
static uint32_t orange_indication_char_add(ble_orange_t * p_orange, const ble_orange_init_t * p_orange_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 0;
    char_md.char_props.indicate = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_orange->uuid_type;
    ble_uuid.uuid = ORANGE_INDICATION_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_INDICATION_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_orange->service_handle,                       
    &char_md,
    &attr_char_value,
    &p_orange->command_res_handles);
}


/**
 ***************************************************************************************************
 *	@brief			Notification Characteristic Setting
 *	@details		Setting Notification Characteristic
 *      @param[in] p_orange  Orange Service structure.
 **************************************************************************************************/
static uint32_t orange_notification_char_add(ble_orange_t * p_orange, const ble_orange_init_t * p_orange_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_orange->uuid_type;
    ble_uuid.uuid = ORANGE_NOTIFICATION_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_NOTIFICATION_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_orange->service_handle,                       
    &char_md,
    &attr_char_value,
    &p_orange->command_res_handles);
}


/**
 ***************************************************************************************************
 *	@brief			Orange Service initialization
 *	@details		Initializing Orange Service
 *      @param[in] p_orange  Orange Service structure.
 **************************************************************************************************/
uint32_t ble_orange_init(ble_orange_t * p_orange, const ble_orange_init_t * p_orange_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_orange->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_orange->orange_write_handler = p_orange_init->orange_write_handler;
    //p_orange->p_orange_state = p_orange_init->p_orange_state;

    // Add service.
    ble_uuid128_t base_uuid = {ORANGE_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_orange->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_orange->uuid_type;
    ble_uuid.uuid = ORANGE_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_orange->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add Lock State characteristic
    err_code = orange_read_char_add(p_orange, p_orange_init);
    VERIFY_SUCCESS(err_code);

    // Add CommandResponse characteristic
    err_code = orange_indication_char_add(p_orange, p_orange_init);
    VERIFY_SUCCESS(err_code);

    // Add CommandResponse characteristic
    err_code = orange_notification_char_add(p_orange, p_orange_init);
    VERIFY_SUCCESS(err_code);

    // Add Lock CommandTransmission characteristic
    err_code = command_write_char_add(p_orange, p_orange_init);
    VERIFY_SUCCESS(err_code);

    return err_code;
}

/**
 ***************************************************************************************************
  Notify Data
 ***************************************************************************************************
 
/**
 ***************************************************************************************************
 *	@brief			Indication transmission function
 *	@details		transmission Indication
 *      @param[in] p_orange  Orange Service structure.
 **************************************************************************************************/
uint32_t ble_orange_notification(ble_orange_t * p_orange, uint8_t *data, uint16_t len)
{
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_orange->command_res_handles.value_handle;
    params.p_data = data;
    params.p_len = &len;

    return sd_ble_gatts_hvx(p_orange->conn_handle, &params);
}

/**
 ***************************************************************************************************
  Indicate Data
 ***************************************************************************************************
 
/**
 ***************************************************************************************************
 *	@brief			Indication transmission function
 *	@details		transmission Indication
 *      @param[in] p_orange  Orange Service structure.
 **************************************************************************************************/
uint32_t ble_orange_indication(ble_orange_t * p_orange, uint8_t *data, uint16_t len)
{
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_INDICATION;
    params.handle = p_orange->command_res_handles.value_handle;
    params.p_data = data;
    params.p_len = &len;

    return sd_ble_gatts_hvx(p_orange->conn_handle, &params);
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/ 
void ble_indication_start(uint8_t ble_type, uint8_t ble_command)
{
    if(true == ble_connect_flag)
    {
        ble_indicate_flag = true;
        ble_indicate_enable = true;
    }
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/ 
static void ble_indication_end(void)
{
    indication_len = 0;
    indication_packet_count = 0;
    indication_packet_num = 0;
    indication_last_packet_mod = 0;
    
    ble_indicate_flag = false;
    ble_indicate_enable = false;
    
    ble_cmd = 0x00;

    for(int i=0; i<BLE_ALL_DATA_MAX; i++)
    {
        indication_buf[i] = 0x00;
    }
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/ 
static uint16_t indication_size;
static void ble_indication_command(void)
{
    uint8_t split_indication_buf[mtu_max_size];
    //uint16_t indication_size;

    split_indication_buf[0] = indication_packet_num;
    split_indication_buf[1] = indication_packet_count;
    for(int i=2; i<mtu_max_size; i++)
    {
        split_indication_buf[i] = indication_buf[(mtu_max_size-2)*indication_packet_count + i - 2];
    }


    if((indication_packet_count+1) == indication_packet_num)
    {
        if(0 != indication_len%(mtu_max_size-2))
        {
            indication_size = indication_len%(mtu_max_size-2)+2; //((128%(200-2) + 2
        } else
        {
            indication_size = mtu_max_size;
        }

    } else
    {
        indication_size = mtu_max_size;
    }

    indication_exe(&split_indication_buf[0], indication_size);
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/ 
void ble_indication_exe(void)
{       
    if(true == ble_indicate_flag)
    {
        if(true == ble_indicate_enable && true == ble_connect_flag)
        {
            ble_indication_command();
            ble_indicate_enable = false;
        }
    }
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/ 
static uint8_t notification_test_cyc = 0;
static uint8_t indication_test_cyc = 0;
static uint8_t send_data[10] = "0123456789";
void peripheral_notification_test(void)
{
    uint8_t indication_size = 10;//sizeof(data);

    if(true == sys_timer_limit[BLE_TEST_TIMER] && true == ble_connect_flag)
    {
        sys_timer_limit[BLE_TEST_TIMER] = false;

        for(int i=0; i<10; i++)
        {
            send_data[i] = '0'+ notification_test_cyc%10;
        }

        notification_exe(&send_data[0], indication_size);

        notification_test_cyc += 1;
    }
}

void peripheral_indication_test(void)
{
    uint8_t indication_size = 10;//sizeof(data);

    if(true == sys_timer_limit[BLE_TEST_TIMER] && true == ble_connect_flag
         && true == ble_indicate_enable)
    {
        sys_timer_limit[BLE_TEST_TIMER] = false;

        for(int i=0; i<10; i++)
        {
            send_data[i] = send_data[i] + indication_test_cyc%10;
        }

        indication_exe(&send_data[0], indication_size);
        ble_indicate_enable = false;

        indication_test_cyc += 1;
    }
}

void peripheral_write_notification_test(void)
{
    if(true == ble_connect_flag && true == write_flag)
    {
        write_flag = false;
        notification_exe(&write_buf[0], received_write_data_len);
    }
}


#endif // NRF_MODULE_ENABLED(BLE_ORANGE)
