/**
 ***************************************************************************************************
 * @file 		lis3dh_ctrl.c
 * @brief 		LIS3DH Control
 ***************************************************************************************************
 **************************************************************************************************/
#define LIS3DH_CTRL_GLOBAL

#include "common.h"

static uint16_t acce_x_data;
static uint16_t acce_y_data;
static uint16_t acce_z_data;

static uint8_t acce_x_buf;
static uint8_t acce_y_buf;
static uint8_t acce_z_buf;

bool LIS3DH_who_am_i(void)
{
    uint8_t read_data;
    twi_init();
    LIS3DH_read(LIS3DH_REG_WHO_AM_I, &read_data);
    twi_stop();

    if (0x33 == read_data)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void LIS3DH_on(void)
{
    twi_init();

    // setting Low Power mode
    // 10Hz ODR[3:0] = 0b0010
    // LPEN enable
    // Xen,Yen,Zem enable
    LIS3DH_write(LIS3DH_REG_CTRL_REG1, 0x2F);
}

void LIS3DH_off(void)
{
    // setting Power down mode
    LIS3DH_write(LIS3DH_REG_CTRL_REG1, 0x0F);

    twi_stop();
}

/********************************************************************************************************************
 * Outline      :
 * Function Name:
 * Description  :
 * Arguments    : None
 * return Value : None
 *********************************************************************************************************************/
void LIS3DH_get_acce(void)
{
	uint8_t temp_data_high;
	uint8_t temp_data_low;

	// X axis
        LIS3DH_read(LIS3DH_REG_OUT_X_L, &temp_data_low);
        LIS3DH_read(LIS3DH_REG_OUT_X_H, &temp_data_high);
	acce_x_data = ((uint16_t)(temp_data_high)<<8) |  (uint16_t)(temp_data_low);

	// Y axis
        LIS3DH_read(LIS3DH_REG_OUT_Y_L, &temp_data_low);
        LIS3DH_read(LIS3DH_REG_OUT_Y_H, &temp_data_high);
	acce_y_data = ((uint16_t)(temp_data_high)<<8) |  (uint16_t)(temp_data_low);

	// Z axis
        LIS3DH_read(LIS3DH_REG_OUT_Z_L, &temp_data_low);
        LIS3DH_read(LIS3DH_REG_OUT_Z_H, &temp_data_high);
	acce_z_data = ((uint16_t)(temp_data_high)<<8) |  (uint16_t)(temp_data_low);
}

void LIS3DH_get_low_acce(void)
{
    // X axis
    LIS3DH_read(LIS3DH_REG_OUT_X_H, &acce_x_buf);

    // Y axis
    LIS3DH_read(LIS3DH_REG_OUT_Y_H, &acce_y_buf);

    // Z axis
    LIS3DH_read(LIS3DH_REG_OUT_Z_H, &acce_z_buf);
}





/***************************************************************************************************
 **************************************************************************************************/