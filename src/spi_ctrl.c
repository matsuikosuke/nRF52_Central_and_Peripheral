/**
 ***************************************************************************************************
 * @file    spi_ctrl.c
 * @brief   SPI Control
 ***************************************************************************************************
 **************************************************************************************************/
#include "common.h"

/*--------------------------------------------------------------------------------------------------
 Static RAM
--------------------------------------------------------------------------------------------------*/
static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(0); /**< SPI instance. */
static volatile bool spi0_trans_done;                      /**< Flag used to indicate that SPI instance completed the transfer. */

/*--------------------------------------------------------------------------------------------------
 Static Function
--------------------------------------------------------------------------------------------------*/

uint8_t SPI_Tx_Buf[SPI_BUFSIZE]; //鷹?
uint8_t SPI_Rx_Buf[SPI_BUFSIZE]; //・謾カ
volatile uint8_t SPIReadLength, SPIWriteLength;

/**
***************************************************************************************************
*	@brief			SPI0 Transfer Event Handler
*	@details		Perform SPI0 transfer completely
**************************************************************************************************/
void spi_event_handler(nrf_drv_spi_evt_t const * p_event, void * p_context)
{
    spi0_trans_done = true;
}

// void spi_event_handler(nrf_drv_spi_evt_t const * p_event,
//                       void *                    p_context)
//{
//    spi_xfer_done = true;
//}

/**
 ***************************************************************************************************
 *	@brief			SPI0 initialization function
 *	@details		Initialize SPI0 module
 **************************************************************************************************/
void spi0_init(void)
{
    if (false == spi_on_flag)
    {
        nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
        spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED; // F_NSS_PIN;  // SPI mode selection (1: SPI idle mode, 0: SPI communication mode)
        spi_config.miso_pin = F_MISO_PIN;             // SPI Serial Data Input (SDI)
        spi_config.mosi_pin = F_MOSI_PIN;             // SPI Serial Data Output (SDO)
        spi_config.sck_pin = F_CLK_PIN;               // SPI Serial Port Clock (SPC)
        spi_config.irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY;
        spi_config.orc = 0xFF;
        spi_config.frequency = NRF_DRV_SPI_FREQ_500K;           // Max of SPI clock frequency is 10MHz
        spi_config.mode = NRF_DRV_SPI_MODE_0;                   // SCK starts from High and data is acquired at the rising edge of SCK
        spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST; // W25Q80DV is MSB(bit7) First
        APP_ERROR_CHECK(nrf_drv_spi_init(&spi0, &spi_config, spi_event_handler, NULL));

        spi_on_flag = true;
    }
}
/*
If the SPI master controls multiple SPI slaves, the CS pin must be controlled manually.
To manually control the CS you can set the ss_pin to NRF_DRV_SPI_PIN_NOT_USED
(spi_config.ss_pin = NRF_DRV_SPI_PIN_NOT_USED) and configure that pin as a GPIO output.
*/

/**
 ***************************************************************************************************
 *	@brief			SPI0 Stop function
 *	@details		Stop SPI0 module
 **************************************************************************************************/
void spi0_stop(void)
{
    if (true == spi_on_flag)
    {
        nrf_drv_spi_uninit(&spi0);

        spi_on_flag = false;
    }
}

/**
 ***************************************************************************************************
 *	@brief			SPI0 Transfer function
 *	@details		Perform SPI0 transfer
 **************************************************************************************************/
void spi0_transfer(uint8_t * write_data, uint8_t * read_data, uint8_t data_length)
{
    spi0_trans_done = false;

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi0, write_data, data_length, read_data, data_length));

    while (!spi0_trans_done)
    {
        // Time Limit Exceeded
        //★：未実装
    }
}

void spi0_write_reg(int data)
{
    spi0_trans_done = false;
    SPIWriteLength = 1;
    SPIReadLength = 0;
    SPI_Tx_Buf[0] = data;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi0, SPI_Tx_Buf, SPIWriteLength, SPI_Rx_Buf, SPIReadLength));
    while(spi0_trans_done == false);
}

uint8_t spi0_read_reg(int reg)
{
    spi0_trans_done = false;
    SPI_Tx_Buf[0] = reg;
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi0, SPI_Tx_Buf, 0, SPI_Rx_Buf, 1));
    while(spi0_trans_done == false);
    return SPI_Rx_Buf[0];
}