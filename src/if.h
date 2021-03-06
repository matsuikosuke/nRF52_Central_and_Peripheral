/**
 ***************************************************************************************************
 * @file        if.h
 * @brief       Intrface Pin Assign
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef IF_H /* IF_H */
#define IF_H

// UART
#define TXD_PIN (25)
#define RXD_PIN (26)

// Accelerometer
#define VIB_INT_PIN (27)

// I2C Common
#define SCL_PIN (12) //P0.12
#define SDA_PIN (11) //P0.11

// DHT11
#define DHT11_BUS_PIN (41) //P1.09 //32+09=41

// SHT31
#define SHT31_SCL_PIN (12) //P0.12
#define SHT31_SDA_PIN (11) //P0.11

// AD
#define LIBAT_TEST_PIN NRF_SAADC_INPUT_AIN0        // AN0
#define EBICYCLE_BAT_TEST_PIN NRF_SAADC_INPUT_AIN1 // AN1
#define TESTBAT_TRIGER_PIN (4)

// FLASH MEMOERY
#define F_MOSI_PIN (15) // SPI Serial Data Output (SDO)
#define F_CLK_PIN (16)  // SPI Serial Port Clock (SPC)
#define F_NSS_PIN (17)  // SPI mode selection (1: SPI idle mode, 0: SPI communication mode)
#define F_MISO_PIN (18) // SPI Serial Data Input (SDI)

// BUZZER
#define DRV_BUZ_PIN (19)

// SW
#define FRONT_SW_PIN (21)

// PWM
#define PWM0_INA_PIN (5)
#define PWM0_INB_PIN (6)



#endif /* IF_H */