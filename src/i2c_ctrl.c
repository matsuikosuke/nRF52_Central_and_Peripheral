/**
 ***************************************************************************************************
 * @file 		i2c_ctrl.c
 * @brief 		I2C Control
 ***************************************************************************************************
 **************************************************************************************************/
#define I2C_CTRL_GLOBAL

#include "common.h"


/* TWI instance ID. */
#define TWI_INSTANCE_ID 1 // 0 is using for FatFs SPIM1

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);


/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
    switch (p_event->type)
    {
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            break;

        case NRF_DRV_TWI_EVT_DONE:
            if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
            {
            }
            m_xfer_done = true;
            break;
        default:
            break;
    }
}


/**
 * @brief TWI initialization.
 */
void twi_init(void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = SCL_PIN,
       .sda                = SDA_PIN,
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    // non-blocking mode
    //    err_code = nrf_drv_twi_init(&m_twi, &twi_config, twi_handler, NULL);
    //    APP_ERROR_CHECK(err_code);

    if (false == i2c_on_flag)
    {
        // blocking mode
        err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
        APP_ERROR_CHECK(err_code);

        nrf_drv_twi_enable(&m_twi);

        i2c_on_flag = true;
    }
}


/**
 ***************************************************************************************************
 *	@brief			SPI0 Stop function
 *	@details		Stop SPI0 module
 **************************************************************************************************/
void twi_stop(void)
{
    if (true == i2c_on_flag)
    {
        nrf_drv_twi_uninit(&m_twi);

        i2c_on_flag = false;
    }
}

/**
 ***************************************************************************************************
 *	@brief			Read LIS3DH function
 *	@details		Read LIS3DH
 **************************************************************************************************/
ret_code_t LIS3DH_read(uint8_t reg_address, uint8_t * read_data)
{
    ret_code_t err_code;


    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, LIS3DH_ADDR, &reg_address, 1, false);
    APP_ERROR_CHECK(err_code);
    //    while (m_xfer_done == false);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    m_xfer_done = false;
    err_code = nrf_drv_twi_rx(&m_twi, LIS3DH_ADDR, read_data, 1);
    APP_ERROR_CHECK(err_code);
    //    while (m_xfer_done == false);

    return err_code;
}

/**
 ***************************************************************************************************
 *	@brief			Write LIS3DH function
 *	@details		Write LIS3DH
 **************************************************************************************************/
ret_code_t LIS3DH_write(uint8_t reg_address, uint8_t write_data)
{
    ret_code_t err_code;
    uint8_t trans_data[2];

    trans_data[0] = reg_address;
    trans_data[1] = write_data;

    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, LIS3DH_ADDR, &trans_data[0], sizeof(trans_data), false);
    APP_ERROR_CHECK(err_code);
    //    while (m_xfer_done == false);

    return err_code;
}

/**
 ***************************************************************************************************
 *	@brief			Read SHT31 function
 *	@details		Read SHT31
 **************************************************************************************************/
ret_code_t SHT31_read(uint8_t reg_address, uint8_t * read_data)
{
    ret_code_t err_code;


    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, SHT31_ADDR, &reg_address, 1, false);
    APP_ERROR_CHECK(err_code);
    //    while (m_xfer_done == false);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    m_xfer_done = false;
    err_code = nrf_drv_twi_rx(&m_twi, SHT31_ADDR, read_data, 1);
    APP_ERROR_CHECK(err_code);
    //    while (m_xfer_done == false);

    return err_code;
}

/**
 ***************************************************************************************************
 *	@brief			Write SHT31 function
 *	@details		Write SHT31
 **************************************************************************************************/
ret_code_t SHT31_write(uint8_t reg_address, uint8_t write_data)
{
    ret_code_t err_code;
    uint8_t trans_data[2];

    trans_data[0] = reg_address;
    trans_data[1] = write_data;

    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, SHT31_ADDR, &trans_data[0], sizeof(trans_data), false);
    APP_ERROR_CHECK(err_code);
    //    while (m_xfer_done == false);

    return err_code;
}

/***************************************************************************************************
 **************************************************************************************************/