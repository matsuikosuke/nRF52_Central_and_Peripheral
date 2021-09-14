/**
 ***************************************************************************************************
 * @file 		w25q80_ctrl.c
 * @brief 		W25Q80DV Control
 ***************************************************************************************************
 **************************************************************************************************/
#define W25Q80DV_CTRL_GLOBAL

#include "common.h"

static void w25q80_check_key(uint8_t key_type, uint8_t *output_buf);
static void w25q80_check_ecdh_key(uint8_t key_type, uint8_t *output_buf);
static void w25q80_check_flash_check(uint8_t *output_buf);

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
uint32_t w25q80_ReadID(void)
{
    uint32_t temp = 0, temp0 = 0, temp1 = 0, temp2 = 0;
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_JEDEC_ID);
    temp0 = spi0_read_reg(W25Q_Dummy_Byte);
    temp1 = spi0_read_reg(W25Q_Dummy_Byte);
    temp2 = spi0_read_reg(W25Q_Dummy_Byte);
    nrf_gpio_pin_set(F_NSS_PIN);
    temp = (temp0 << 16) | (temp1 << 8) | temp2;
    return temp;
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_PowerDown()
{
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_Powerdown);
    nrf_gpio_pin_set(F_NSS_PIN);
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_ReleasePowerDown()
{
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_ReleasePowerdown);
    nrf_gpio_pin_set(F_NSS_PIN);
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_WriteEnable()
{
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_WRITE_ENABLE);
    nrf_gpio_pin_set(F_NSS_PIN);
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
uint8_t w25q80_ReadStatus1()
{
    unsigned char FLASH_Status = 0;
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_ReadStatus1);
    FLASH_Status = spi0_read_reg(W25Q_Dummy_Byte);
    nrf_gpio_pin_set(F_NSS_PIN);

    return FLASH_Status;
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
uint8_t w25q80_ReadStatus2()
{
    unsigned char FLASH_Status = 0;
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_ReadStatus2);
    FLASH_Status = spi0_read_reg(W25Q_Dummy_Byte);
    nrf_gpio_pin_set(F_NSS_PIN);

    return FLASH_Status;
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_WaitForWriteEnd()
{
    unsigned char FLASH_Status = 0;
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_ReadStatus1);
    do
    {
        FLASH_Status = spi0_read_reg(W25Q_Dummy_Byte);
    } while ((BUSY_Flag & FLASH_Status) == 1);
    nrf_gpio_pin_set(F_NSS_PIN);
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_WaitForWelEnd()
{
    unsigned char FLASH_Status = 0;
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_ReadStatus1);
    do
    {
        FLASH_Status = spi0_read_reg(W25Q_Dummy_Byte);
    } while ((WEL_Flag & FLASH_Status) == 1);
    nrf_gpio_pin_set(F_NSS_PIN);
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_FLASH_SectorUnlock(uint32_t SectorAddr)
{
    w25q80_WriteEnable();
    w25q80_WaitForWriteEnd();
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(0x39);
    spi0_write_reg((SectorAddr & 0XFF0000) >> 16);
    spi0_write_reg((SectorAddr & 0XFF00) >> 8);
    spi0_write_reg(SectorAddr & 0XFF);
    nrf_gpio_pin_set(F_NSS_PIN);
    w25q80_WaitForWriteEnd();
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_FLASH_SectorErase(uint32_t SectorAddr)
{
    w25q80_WriteEnable();
    // There is an instruction in the protocol, 
    // but it cannot be deleted without deleting it.
    //w25q80_WaitForWriteEnd(); 
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_SectorErase4KB);
    spi0_write_reg((SectorAddr & 0XFF0000) >> 16);
    spi0_write_reg((SectorAddr & 0XFF00) >> 8);
    spi0_write_reg(SectorAddr & 0XFF);
    nrf_gpio_pin_set(F_NSS_PIN);
    w25q80_WaitForWriteEnd();
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_FLASH_PageWrite(unsigned char * pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    w25q80_WriteEnable();
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_PageProgram);
    spi0_write_reg((WriteAddr & 0XFF0000) >> 16);
    spi0_write_reg((WriteAddr & 0XFF00) >> 8);
    spi0_write_reg(WriteAddr & 0XFF);
    if (NumByteToWrite > 256)
    {
        return;
    }
    while (NumByteToWrite--)
    {
        spi0_write_reg(*pBuffer);
        pBuffer++;
    }
    nrf_gpio_pin_set(F_NSS_PIN);
    w25q80_WaitForWriteEnd();
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_Flash_BufferRead(uint8_t * pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_ReadData);
    spi0_write_reg((ReadAddr & 0XFF0000) >> 16);
    spi0_write_reg((ReadAddr & 0XFF00) >> 8);
    spi0_write_reg(ReadAddr & 0XFF);
    while (NumByteToRead--)
    {
        *pBuffer = spi0_read_reg(W25Q_Dummy_Byte);
        pBuffer++;
    }
    nrf_gpio_pin_set(F_NSS_PIN);
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_Chip_Erase()
{
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_WriteEnable();
    nrf_gpio_pin_clear(F_NSS_PIN);
    spi0_write_reg(W25Q_REG_ChipErase);
    nrf_gpio_pin_set(F_NSS_PIN);
    w25q80_WaitForWriteEnd();

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
bool w25q80_save_public_key(uint8_t key_type, uint8_t *input_buf)
{
    uint16_t start_index = 0;
    uint16_t end_index = 0;
    uint16_t input_len = 0;
    uint16_t page_num = 0;
    uint8_t check_begin[] = "BEGIN";
    uint8_t check_end[] = "END";
    uint8_t test_buf[600] = {0};

    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_FLASH_SectorErase(W25Q_SECTOR_START*key_type);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    for(int i=5; i<RECEIVE_BUF_MAX; i++)
    {

        if(*(input_buf+i) == *check_begin && *(input_buf+i+1) == *(check_begin+1) && *(input_buf+i+2) == *(check_begin+2) 
            && *(input_buf+i+3) == *(check_begin+3) && *(input_buf+i+4) == *(check_begin+4))
        {
            start_index = i-5;
            break;
        }
    }

    for(int i=0; i<RECEIVE_BUF_MAX; i++)
    {
        if(*(input_buf+i) == *check_end && *(input_buf+i+1) == *(check_end+1) && *(input_buf+i+2) == *(check_end+2) )
        {
            if(SERVER_PRIMARY_KEY == key_type || SERVER_SECONDARY_KEY == key_type || CHARI_PUBLIC_KEY== key_type)
            {
                end_index = i+NUM_FROM_END_PUBLIC_KEY;
            } else if(CHARI_PRIVATE_KEY == key_type)
            {
                end_index = i+NUM_FROM_END_PRIVATE_KEY;
            }

            break;
        }
    }

    if(0 != end_index)
    {
        input_len = end_index - start_index + 1;
        page_num = input_len / W25Q_PAGE_MAX;
        for(int i=0; i<page_num; i++)
        {
            w25q80_FLASH_PageWrite(input_buf+start_index+W25Q_PAGE_MAX*i, W25Q_PAGE_START*(W25Q_SECTOR_MAX*key_type+i), W25Q_PAGE_MAX);
            nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);
        }

        
        if(0 != (input_len % W25Q_PAGE_MAX))
        {
            w25q80_FLASH_PageWrite(input_buf+start_index+W25Q_PAGE_MAX*page_num, W25Q_PAGE_START*(W25Q_SECTOR_MAX*key_type+page_num), (input_len % W25Q_PAGE_MAX));
            nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);
        }

        // check write data
        w25q80_check_key(key_type, test_buf);

        if(memcmp( (input_buf+start_index), test_buf, input_len ) == 0)
        {
            w25q80_PowerDown();
            nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

            spi0_stop();
            return true;
        }
    }

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();
    return false;
}




/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
bool w25q80_save_ecdh_key(uint8_t key_type, uint8_t *input_buf)
{
    uint8_t test_buf[32] = {0};

    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_FLASH_SectorErase(W25Q_SECTOR_START*key_type);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_FLASH_PageWrite(input_buf, W25Q_PAGE_START*(W25Q_SECTOR_MAX*key_type), 32);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);        

    // check write data
    w25q80_check_ecdh_key(key_type, test_buf);

    if(memcmp( input_buf, test_buf, 32 ) == 0)
    {
        w25q80_PowerDown();
        nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

        spi0_stop();
        return true;
    }

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();
    return false;
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
static void w25q80_check_key(uint8_t key_type, uint8_t *output_buf)
{    
    uint8_t check_end[] = "END";
    uint16_t end_index = 0;
    uint8_t temp_buf[600] ={0};

    for(int i=0; i<2; i++)
    {
        w25q80_Flash_BufferRead(temp_buf+W25Q_PAGE_MAX*i, W25Q_PAGE_START*(W25Q_SECTOR_MAX*key_type+i), W25Q_PAGE_MAX);
        nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);
    }
    
    for(int i=0; i<RECEIVE_BUF_MAX; i++)
    {
        if(*(temp_buf+i) == *check_end && *(temp_buf+i+1) == *(check_end+1) && *(temp_buf+i+2) == *(check_end+2) )
        {
            if(SERVER_PRIMARY_KEY == key_type || SERVER_SECONDARY_KEY == key_type || CHARI_PUBLIC_KEY== key_type)
            {
                end_index = i+NUM_FROM_END_PUBLIC_KEY;
            } else if(CHARI_PRIVATE_KEY == key_type)
            {
                end_index = i+NUM_FROM_END_PRIVATE_KEY;
            }
            break;
        }
    }

    for(int i=0; i<end_index+1; i++)
    {
        *(output_buf+i) = *(temp_buf+i);
    }

//    *(output_buf+end_index+1) = '\r';
//    *(output_buf+end_index+2) = '\n';
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_read_key(uint8_t key_type, uint8_t *output_buf)
{    
    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    // check write data
    w25q80_check_key(key_type, output_buf);

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
static void w25q80_check_ecdh_key(uint8_t key_type, uint8_t *output_buf)
{    
    w25q80_Flash_BufferRead(output_buf, W25Q_PAGE_START*(W25Q_SECTOR_MAX*key_type), ECDH_KEY_SIZE);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);    
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_read_ecdh_key(uint8_t key_type, uint8_t *output_buf)
{    
    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    // check write data
    w25q80_check_ecdh_key(key_type, output_buf);

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();
}



/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_check_memory(void)
{
    uint8_t my_ecdh_public_key[ECDH_KEY_SIZE] = {0};
    uint8_t my_ecdh_private_key[ECDH_KEY_SIZE] = {0};
    uint8_t rsa_primary_key[450] = {0};
    uint8_t rsa_secondary_key[450] = {0};
    
    uint8_t test_buf[FLASH_CHECK_LENGTH] = FLASH_CHECK;
    uint8_t read_buf[FLASH_CHECK_LENGTH] = {0};

    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    // check write data
    w25q80_check_flash_check(read_buf);

    if(memcmp( test_buf, read_buf, FLASH_CHECK_LENGTH ) != 0)
    {
    
        w25q80_PowerDown();
        nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

        spi0_stop();

        flash_check =  false;
        w25q80_Chip_Erase();
        return;
    }

    w25q80_check_ecdh_key(CHARI_ECDH_PUBLIC_KEY, my_ecdh_public_key);
    w25q80_check_ecdh_key(CHARI_ECDH_PRIVATE_KEY, my_ecdh_private_key);
    
    w25q80_check_key(SERVER_PRIMARY_KEY, rsa_primary_key);
    w25q80_check_key(SERVER_SECONDARY_KEY, rsa_secondary_key);

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();

    // ECDH Public Key確認
    uint16_t empty_count = 0;
    for(int i=0; i<ECDH_KEY_SIZE; i++)
    {
        if(0xFF == my_ecdh_public_key[i])
        {
            empty_count += 1;
        }
    }

    if(ECDH_KEY_SIZE == empty_count)
    {
        empty_count += 1;
        flash_check =  false;
        w25q80_Chip_Erase();
        return;
    }    

    // ECDH Private Key確認
    empty_count = 0;
    for(int i=0; i<ECDH_KEY_SIZE; i++)
    {
        if(0xFF == my_ecdh_private_key[i])
        {
            empty_count += 1;
        }
    }

    if(ECDH_KEY_SIZE == empty_count)
    {
        flash_check =  false;
        w25q80_Chip_Erase();
        return;
    }

    // Primary Key確認
    empty_count = 0;
    for(int i=0; i<ECDH_KEY_SIZE; i++)
    {
        if(0x00 == rsa_primary_key[i+10])
        {
            empty_count += 1;
        }
    }

    if(ECDH_KEY_SIZE == empty_count)
    {
        flash_check =  false;
        w25q80_Chip_Erase();
        return;
    }


    // Secondary Key確認
    empty_count = 0;
    for(int i=0; i<ECDH_KEY_SIZE; i++)
    {
        if(0x00 == rsa_secondary_key[i+10])
        {
            empty_count += 1;
        }
    }

    if(ECDH_KEY_SIZE == empty_count)
    {
        flash_check =  false;
        w25q80_Chip_Erase();
        return;
    }

    flash_check =  true;
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
void w25q80_erase_ecdh_key(void)
{
    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_FLASH_SectorErase(W25Q_SECTOR_START*CHARI_ECDH_PUBLIC_KEY);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_FLASH_SectorErase(W25Q_SECTOR_START*CHARI_ECDH_PRIVATE_KEY);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();
}


/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
static void w25q80_check_flash_check(uint8_t *output_buf)
{    
    w25q80_Flash_BufferRead(output_buf, W25Q_PAGE_START*(W25Q_SECTOR_MAX*FLASH_CHECK_AREA), FLASH_CHECK_LENGTH);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);    
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
bool w25q80_read_flash_check(void)
{    
    uint8_t test_buf[FLASH_CHECK_LENGTH] = FLASH_CHECK;
    uint8_t read_buf[FLASH_CHECK_LENGTH] = {0};

    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    // check write data
    w25q80_check_flash_check(read_buf);

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();

    if(memcmp( test_buf, read_buf, FLASH_CHECK_LENGTH ) == 0)
    {
        return true;
    }

    return false;
}

/**
 ***************************************************************************************************
 *	@brief			//
 *	@details		//
 **************************************************************************************************/
bool w25q80_save_flash_check(void)
{
    uint8_t write_buf[FLASH_CHECK_LENGTH] = FLASH_CHECK;
    uint8_t read_buf[FLASH_CHECK_LENGTH] = {0};

    // SPI initialize setting
    spi0_init();

    w25q80_ReleasePowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_FLASH_SectorErase(W25Q_SECTOR_START*FLASH_CHECK_AREA);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    w25q80_FLASH_PageWrite(write_buf, W25Q_PAGE_START*(W25Q_SECTOR_MAX*FLASH_CHECK_AREA), FLASH_CHECK_LENGTH);
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);        

    // check write data
    w25q80_check_flash_check(read_buf);

    if(memcmp( write_buf, read_buf, FLASH_CHECK_LENGTH ) == 0)
    {
        w25q80_PowerDown();
        nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

        spi0_stop();
        return true;
    }

    w25q80_PowerDown();
    nrf_delay_ms(WAIT_FOR_NEXT_W25CMD);

    spi0_stop();
    return false;
}

/***************************************************************************************************
 **************************************************************************************************/