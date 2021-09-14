/**
 ***************************************************************************************************
 * @file 		uart_ctrl.c
 * @brief 		UART Control
 ***************************************************************************************************
 **************************************************************************************************/
#define UART_CTRL_GLOBAL

#include "common.h"

#include "app_fifo.h"
#include "app_uart.h"
#include "nrf_drv_uart.h"
#if defined(UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined(UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

/*--------------------------------------------------------------------------------------------------
-	Static Function
--------------------------------------------------------------------------------------------------*/
static void receive_message(uint8_t * data_buf);

/**
 ***************************************************************************************************
 * @brief		UART event handler
 * @details		Called when an interrupt related to UART occurs
 *
 * @param[in]	        p_event Pointer to the structure that stores UART event information
 * @return		None
 **************************************************************************************************/
static void uart_event_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_DATA)
    { /* 受信割込み */
    }
    else if (p_event->evt_type == APP_UART_TX_EMPTY)
    { /* 送信割込み */
    }
    else if (p_event->evt_type == APP_UART_DATA_READY)
    { /* 受信割込み */
        receive_message(&receive_data[receive_index]);
    }
    else if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR) /* UART通信エラー */ /* app_uart.cを使用時? */
    {
        APP_ERROR_HANDLER(p_event->data.error_communication); // エラーハンドラの中でリセットするのでコメントアウト
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR) /* FIFOエラー */ /* app_uart_fifo.cを使用時？ */
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
}


/**
 ***************************************************************************************************
 * @brief		UART initialization processing
 * @details		Set UART Port and Baudrate
 **************************************************************************************************/
void uart_init_ec(void)
{
  uint32_t err_code;

  const app_uart_comm_params_t comm_params =
  {
          RXD_PIN,
          TXD_PIN,
          NULL,
          NULL,
          APP_UART_FLOW_CONTROL_DISABLED,
          false,
#if defined (UART_PRESENT)
          NRF_UART_BAUDRATE_115200
#else
          NRF_UARTE_BAUDRATE_115200
#endif
	};	

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_event_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);

    uart_on_flag = true;
}

/**
 ***************************************************************************************************
 * @brief		UART stop
 * @details		flushing the RX and TX buffers and close UART
 **************************************************************************************************/
void uart_stop(void)
{
    app_uart_flush(); // flushing the RX and TX buffers
    app_uart_close(); // closing the UART module

    uart_on_flag = false;
}

/**
 ***************************************************************************************************
 * @brief		UART FIFO 1 byte receive
 * @details		Receive 1 Byte from UART FIFO
 *
 * @param[in,out]       data uint8_t * type variable or array pointer
 * @return		variable or array that stores the value
 **************************************************************************************************/
uint32_t uart_fifo_get( uint8_t* data )
{
    return( app_uart_get( data ) );
}

/**
 ***************************************************************************************************
 * @brief		UART FIFO 1 byte transmission
 * @details		Transmit 1 Byte from UART FIFO
 * @param[in]	        data uint8_t * type variable or array pointer
 * @return		error code
 **************************************************************************************************/
uint32_t uart_fifo_put( uint8_t data )
{
    return( app_uart_put( data ) );
}

/**
 ***************************************************************************************************
 * @brief		UART Message transmission
 * @details		Transmit Message.
 *                      Transmission ends when there is no data in the transmission buffer
 *                      or a line feed code is transmitted.
 * @param[in]	        data uint8_t * type variable or array pointer
 * @return		error code
 **************************************************************************************************/
uint32_t send_message(uint8_t * data)
{
    uint32_t err_code;
    uint8_t * cr_data = "\r";
    uint8_t * lf_data = "\n";
    uint8_t * ok_data = "OK";

    while (1)
    {
        err_code = uart_fifo_put(*data);

        if (*data == *lf_data)
        {
            break;
        }

        data += 1;

        if (*data == NULL)
        {
            break;
        }
    }

    return err_code;
}

uint32_t send_characters(uint8_t * data, int length)
{
    uint32_t err_code = NRF_SUCCESS;

    for(int i=0; i<length; i++)
    {
        err_code = uart_fifo_put(*data);
        data += 1;

        if (*data == NULL)
        {
            break;
        }
    }

    return err_code;
}

/**
 ***************************************************************************************************
 * @brief		UART Message receive
 * @details		Receive Message.
 * @param[in]	        data uint8_t * type variable or array pointer
 * @return		error code
 **************************************************************************************************/
static void receive_message(uint8_t * data_buf)
{
    while (app_uart_get(&receive_data[receive_index]) != NRF_SUCCESS);

    receive_index += 1; 
    if(RECEIVE_BUF_MAX == receive_index)
    {
        receive_index = 0;
    }
}


/***************************************************************************************************
 **************************************************************************************************/