/**
 ***************************************************************************************************
 * @file 		spi_ctrl.h
 * @brief 		SPI Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef SPI_CTRL_H_ /* SPI_CTRL_H_ */
#define SPI_CTRL_H_

/**
 * @brief	extern declaration
 */
#ifdef SPI_CTRL_GLOBAL
#define SPI_CTRL_EXTERN
#else
#define SPI_CTRL_EXTERN extern
#endif


/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
SPI_CTRL_EXTERN void spi0_init(void);
SPI_CTRL_EXTERN void spi0_stop(void);
SPI_CTRL_EXTERN void spi0_transfer(uint8_t * write_data, uint8_t * read_data, uint8_t data_length);
SPI_CTRL_EXTERN void spi0_write_reg(int data);
SPI_CTRL_EXTERN uint8_t spi0_read_reg(int reg);

#endif /* SPI_CTRL_H_ */
/***************************************************************************************************
 **************************************************************************************************/