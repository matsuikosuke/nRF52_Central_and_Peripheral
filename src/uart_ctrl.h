/**
 ***************************************************************************************************
 * @file 		uart_ctrl.h
 * @brief 		UART Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef UART_CTRL_H_ /* UART_CTRL_H_ */
#define UART_CTRL_H_

/**
 * @brief	extern declaration
 */
#ifdef UART_CTRL_GLOBAL
#define UART_CTRL_EXTERN
#else
#define UART_CTRL_EXTERN extern
#endif

#include "app_uart.h"

/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
UART_CTRL_EXTERN void uart_init_ec(void);
UART_CTRL_EXTERN void uart_stop(void);
UART_CTRL_EXTERN uint32_t uart_fifo_get(uint8_t * data);
UART_CTRL_EXTERN uint32_t uart_fifo_put(uint8_t data);
UART_CTRL_EXTERN uint32_t send_message(uint8_t * data);
UART_CTRL_EXTERN uint32_t send_characters(uint8_t * data, int length);
UART_CTRL_EXTERN uint8_t wait_reception(void);
UART_CTRL_EXTERN uint8_t wait_ok(void);
UART_CTRL_EXTERN uint8_t wait_pub(void);
UART_CTRL_EXTERN bool wait_trans(void);


#endif /* UART_CTRL_H_ */
/***************************************************************************************************
 **************************************************************************************************/