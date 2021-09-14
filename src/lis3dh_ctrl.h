/**
 ***************************************************************************************************
 * @file 		lis3dh_ctrl.h
 * @brief 		LIS3DH Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef LIS3DH_CTRL_H_ /* UART_CTRL_H_ */
#define LIS3DH_CTRL_H_

/**
 * @brief	extern declaration
 */
#ifdef LIS3DH_CTRL_GLOBAL
#define LIS3DH_CTRL_EXTERN
#else
#define LIS3DH_CTRL_EXTERN extern
#endif


/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
LIS3DH_CTRL_EXTERN bool LIS3DH_who_am_i(void);
LIS3DH_CTRL_EXTERN void LIS3DH_on(void);
LIS3DH_CTRL_EXTERN void LIS3DH_off(void);
LIS3DH_CTRL_EXTERN void LIS3DH_get_acce(void);
LIS3DH_CTRL_EXTERN void LIS3DH_get_low_acce(void);

#endif /* UART_CTRL_H_ */
/***************************************************************************************************
 **************************************************************************************************/