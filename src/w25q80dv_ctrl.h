/**
 ***************************************************************************************************
 * @file 		w25q80_ctrl.h
 * @brief 		W25Q80DV Control
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef W25Q80_CTRL_H_ /* UART_CTRL_H_ */
#define W25Q80_CTRL_H_

/**
 * @brief	extern declaration
 */
#ifdef W25Q80_CTRL_GLOBAL
#define W25Q80_CTRL_EXTERN
#else
#define W25Q80_CTRL_EXTERN extern
#endif
#include <string.h>

/*--------------------------------------------------------------------------------------------------
- Function Declaration
--------------------------------------------------------------------------------------------------*/
W25Q80_CTRL_EXTERN uint32_t w25q80_ReadID(void);
W25Q80_CTRL_EXTERN void w25q80_PowerDown();
W25Q80_CTRL_EXTERN void w25q80_ReleasePowerDown();
W25Q80_CTRL_EXTERN void w25q80_WriteEnable(void);
W25Q80_CTRL_EXTERN uint8_t w25q80_ReadStatus1(void);
W25Q80_CTRL_EXTERN uint8_t w25q80_ReadStatus2(void);
W25Q80_CTRL_EXTERN void w25q80_WaitForWriteEnd(void);
W25Q80_CTRL_EXTERN void w25q80_WaitForWelEnd();
W25Q80_CTRL_EXTERN void w25q80_FLASH_SectorUnlock(uint32_t SectorAddr);
W25Q80_CTRL_EXTERN void w25q80_FLASH_SectorErase(uint32_t SectorAddr);
W25Q80_CTRL_EXTERN void w25q80_FLASH_PageWrite(unsigned char * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
W25Q80_CTRL_EXTERN void w25q80_Flash_BufferRead(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
W25Q80_CTRL_EXTERN void w25q80_Chip_Erase(void);
W25Q80_CTRL_EXTERN bool w25q80_save_public_key(uint8_t key_type, uint8_t *input_buf);
W25Q80_CTRL_EXTERN bool w25q80_save_ecdh_key(uint8_t key_type, uint8_t *input_buf);
W25Q80_CTRL_EXTERN bool w25q80_save_json(uint8_t *input_buf);
W25Q80_CTRL_EXTERN void w25q80_read_key(uint8_t key_type, uint8_t *output_buf);
W25Q80_CTRL_EXTERN void w25q80_read_ecdh_key(uint8_t key_type, uint8_t *output_buf);
W25Q80_CTRL_EXTERN void w25q80_check_memory(void);
W25Q80_CTRL_EXTERN void w25q80_erase_ecdh_key(void);
W25Q80_CTRL_EXTERN bool w25q80_read_flash_check(void);
W25Q80_CTRL_EXTERN bool w25q80_save_flash_check(void);

#endif /* UART_CTRL_H_ */
/***************************************************************************************************
 **************************************************************************************************/