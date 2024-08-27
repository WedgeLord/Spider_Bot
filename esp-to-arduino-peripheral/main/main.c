#include <stdio.h>
#include <string.h>
#include "driver/i2c_slave.h"

// #define INCLUDE_vTaskDelay 1
// #include "freertos/task.h"

bool receiveFromNano( i2c_slave_dev_handle_t, const i2c_slave_rx_done_event_data_t *, void * );

void app_main(void)
{
    i2c_slave_config_t esp_config = {
        .i2c_port   = -1,   // -1 for auto-select
        .sda_io_num = 4,
        .scl_io_num = 5,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = 32,   
        .slave_addr = 0xCD,
        .addr_bit_len = I2C_ADDR_BIT_LEN_7,
        .intr_priority = 0, // 0 for default interrupt priority
    };
    // Set up peripheral device (esp32) on the bus
    i2c_slave_dev_handle_t p_handle;
    ESP_ERROR_CHECK( i2c_new_slave_device( &esp_config, &p_handle ) );
    char rx_msg[32], tx_msg[32];
    i2c_slave_event_callbacks_t rx_callback = { // called whenever rx interrupt is triggered
        receiveFromNano 
    };
    ESP_ERROR_CHECK( i2c_slave_register_event_callbacks( p_handle, &rx_callback, rx_msg ) );

    strcpy( tx_msg, "tour" );
    ESP_ERROR_CHECK( i2c_slave_transmit( p_handle, (uint8_t*)tx_msg, 4, 1000 ) );
    for (int i = 0; i < 100000; i++) ;
    ESP_ERROR_CHECK( i2c_slave_transmit( p_handle, (uint8_t*) "this is a test!", 11, 1000 ) );
    memset(tx_msg, 0, 4);
    printf( "\"test\": %X\n", *((int*)"test") );
}

bool receiveFromNano( i2c_slave_dev_handle_t p_handle, const i2c_slave_rx_done_event_data_t *event, void *buffer ) {
    i2c_slave_receive( p_handle, buffer, 32 );
    printf( "from Arduino: \"%32s\"\n", (char*)buffer );
    return false;
}