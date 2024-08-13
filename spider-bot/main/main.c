#include <stdio.h>
#define MASTER
#ifdef MASTER
#include "driver/i2c_master.h"

void app_main(void)
{
    //setup 
    i2c_master_bus_config_t bus_config = {
        .i2c_port       = -1,
        .sda_io_num     = 4,
        .scl_io_num     = 5,
        .clk_source     = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true    // ".flags" missing from IDF docs :(
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(
        i2c_new_master_bus( &bus_config, &bus_handle )
    );

    i2c_device_config_t dev_config = {
        .dev_addr_length    = I2C_ADDR_BIT_LEN_7,
        .device_address     = 0x55,
        .scl_speed_hz       = 100000,
    };

    i2c_master_dev_handle_t device;
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device( bus_handle, &dev_config, &device )
    );

    for (int i = 8; i < 127; i++ ) {
        if ( ESP_OK == i2c_master_probe( bus_handle, i, 100 ) )
            printf( "device found at 0x%X\n", i);
    }
    char msg[32] = {0};
    ESP_ERROR_CHECK(
        i2c_master_receive( device, (uint8_t *)msg, 8, 1000)
    );
    puts( msg );
    while (1)
    ESP_ERROR_CHECK( 
        i2c_master_transmit( device, (uint8_t *)"hey!", 5, -1)
    );
}

#else
#include "driver/i2c_slave.h"

void app_main(void)
{
    // setup
    i2c_slave_config_t per_config = {
        .i2c_port       = -1,
        .sda_io_num     = 4,
        .scl_io_num     = 5,
        .clk_source     = I2C_CLK_SRC_DEFAULT,
        .send_buf_depth = 32,
        .slave_addr     = 0x55,
        .addr_bit_len   = I2C_ADDR_BIT_LEN_7,
    };

    i2c_slave_dev_handle_t peripheral;
    ESP_ERROR_CHECK( 
        i2c_new_slave_device( &per_config, &peripheral )
    );

    ESP_ERROR_CHECK(
        i2c_slave_transmit( peripheral, (uint8_t *)"hey!", 5, -1 )
    );
    
    char msg[32];
    while (1) {
        ESP_ERROR_CHECK(
            i2c_slave_receive( peripheral, (uint8_t *)msg, 8 )
        );
        printf( "%s", msg );
        msg[0] = '\0';
    }
}
#endif
