/**
 ***************************************************************************************************
 * @file 		i2c_ctrl.h
 * @brief 		I2C Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef I2C_CTRL_H_ /* I2C_CTRL_H_ */
#define I2C_CTRL_H_

/**
 * @brief	extern declaration
 */
#ifdef I2C_CTRL_GLOBAL
#define I2C_CTRL_EXTERN
#else
#define I2C_CTRL_EXTERN extern
#endif

/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
I2C_CTRL_EXTERN void twi_init(void);
I2C_CTRL_EXTERN void twi_stop(void);
I2C_CTRL_EXTERN ret_code_t LIS3DH_read(uint8_t reg_address, uint8_t * read_data);
I2C_CTRL_EXTERN ret_code_t LIS3DH_write(uint8_t reg_address, uint8_t write_data);

#endif /* I2C_CTRL_H_ */
/***************************************************************************************************
 **************************************************************************************************/