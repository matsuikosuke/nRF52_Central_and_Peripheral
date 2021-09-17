#include "common.h"

/**@brief Function for application main entry.
 */
int main(void)
{
    ret_code_t err_code;
    ret_code_t ret_val;

    // Initialize.
#if defined(WITHOUT_DFU)
#else
    // Initialize the async SVCI interface to bootloader before any interrupts are enabled.
    err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);
#endif

    cpu_init();

    power_management_init();

    ble_init();

    // Timer start
    system_timer_start();

    sys_timer_flag[BLE_TEST_TIMER] = true;
    ble_indicate_enable = true;

    // Enter main loop.
    for (;;)
    {
        //Test
        central_write_notification_test();
        peripheral_write_notification_test();
//        peripheral_notification_test();
        central_read_test();

        WDT_RESTART();
        idle_state_handle();

    }
}


/**
 * @}
 */