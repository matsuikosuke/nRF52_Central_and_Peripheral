/**
 ***************************************************************************************************
 * @file 		dht11_ctrl.c
 * @brief 		DHT11 Control
 ***************************************************************************************************
 **************************************************************************************************/
#define DHT11_CTRL_GLOBAL

#include "common.h"

static void DHT11_start(void)
{
    // setting OUTPUT
    nrf_gpio_cfg_output(DHT11_BUS_PIN);

    nrf_gpio_pin_set(DHT11_BUS_PIN);
    nrf_delay_ms(200);
    nrf_gpio_pin_clear(DHT11_BUS_PIN);
    nrf_delay_ms(30); // min:10ms, typ:20ms, max:50ms
    nrf_gpio_pin_set(DHT11_BUS_PIN);
    nrf_delay_us(40);
}

static bool DHT11_ready_for(void)
{
    uint8_t count = 0;

    // setting INPUT
    nrf_gpio_cfg_input(DHT11_BUS_PIN, NRF_GPIO_PIN_PULLUP);

    // Low入力待機
    count = 0;
    while(1 == nrf_gpio_pin_read(DHT11_BUS_PIN))
    {
        nrf_delay_us(1);
        count += 1;
        if(count > 200)
            return false;
    }

    // High入力待機
    count = 0;
    while(0 == nrf_gpio_pin_read(DHT11_BUS_PIN))
    {
        nrf_delay_us(1);
        count += 1;
        if(count > 200)
            return false;
    }

    // Low入力待機
    count = 0;
    while(1 == nrf_gpio_pin_read(DHT11_BUS_PIN))
    {
        nrf_delay_us(1);
        count += 1;
        if(count > 200)
            return false;
    }

    return true;
}

// 端子設定が入力で、端子状態が0で開始することを前提とする
static uint8_t DHT11_get_bit(void)
{
    uint8_t count = 0;

    // High入力待機
    count = 0;
    while(0 == nrf_gpio_pin_read(DHT11_BUS_PIN))
    {
        nrf_delay_us(1);
        count += 1;
        if(count > 200)
            return 0xFF;
    }
    
    // Low入力待機
    count = 0;
    while(1 == nrf_gpio_pin_read(DHT11_BUS_PIN))
    {
        nrf_delay_us(1);
        count += 1;
        if(count > 200)
            return 0xFF;         
    }

    // bit 0 or 1 判定
//    if(count > 65 && count < 75)
//        return 1;
//    else if(count > 20 && count < 28)
//        return 0;
//    else
//        return 0xFF;
    if(count > 28)
        return 1;
    else if(count > 10)
        return 0;
    else
        return 0xFF;
}

static uint8_t DHT11_get_byte(void)
{
    uint8_t byte_data = 0;
    uint8_t bit_data = 0;

    for(int i=0; i<8; i++)
    {
        bit_data = DHT11_get_bit();
        if(0xFF == bit_data)
            return 0xFF;
        else
            byte_data |= (bit_data) << (7-i);
    }

    return byte_data;
}

static void DHT11_end(void)
{
    // setting OUTPUT
    nrf_gpio_cfg_output(DHT11_BUS_PIN);

    nrf_gpio_pin_set(DHT11_BUS_PIN);
}

static bool DHT11_get_data(void)
{
    uint8_t dht11_data_buf[5] = {0};

    DHT11_start();
    if(false == DHT11_ready_for())
    {
        DHT11_end();
        return false;
    }

    for(int i=0; i<5; i++)
    {
        dht11_data_buf[i] = DHT11_get_byte();
        if(0xFF == dht11_data_buf[i])
        {
            nrf_delay_ms(10); //センサからの送信完了を待機
            DHT11_end();
            return false;
        }
    }

    // データ取得成功した場合
    humidity_high    = dht11_data_buf[0];
    humidity_low     = dht11_data_buf[1];
    temperature_high = dht11_data_buf[2];
    temperature_low  = dht11_data_buf[3];
    parity           = dht11_data_buf[4];

    DHT11_end();
    return true;
}

static uint8_t dht11_error_count;
void DHT11_certainty_get_data(void)
{
    uint8_t retry_count = 0;

    while(1)
    {        
        //　エラーの場合、100回までやり直す
        if(false == DHT11_get_data())
        {
            retry_count += 1;
            if(100 == retry_count)
            {
                dht11_error_count += 1;
                return;
            }
        }
        else
        {
            printf("temperature is %d.%d C\n",temperature_high, temperature_low);
            printf("humidity is %d.%d %RH\n",humidity_high, humidity_low);
            return;
        }
    }
}

/***************************************************************************************************
 **************************************************************************************************/