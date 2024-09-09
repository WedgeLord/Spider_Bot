#include <stdio.h>
#include <string.h>
#include "esp_log.h"
// #define I2C
#ifdef I2C
#include "driver/i2c_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

i2c_master_dev_handle_t device;

void send_get() {
    //setup 
    i2c_master_bus_config_t bus_config = {
        .i2c_port       = -1,
        .sda_io_num     = 4,
        .scl_io_num     = 5,
        .clk_source     = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        // used if you don't have physical uexternal pull-up resistors. System might not work using just internal pullups
        .flags.enable_internal_pullup = false    // ".flags" missing from IDF docs :(
    };

    i2c_master_bus_handle_t bus_handle; 
    ESP_ERROR_CHECK(
        i2c_new_master_bus( &bus_config, &bus_handle )
    );

    i2c_device_config_t dev_config = {
        .dev_addr_length    = I2C_ADDR_BIT_LEN_7,
        .device_address     = 0x55,
        .scl_speed_hz       = 10000,
        .scl_wait_us        = 1000,
        .flags.disable_ack_check    = 0,
    };

    // i2c_master_dev_handle_t device;
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device( bus_handle, &dev_config, &device )
    );

    // for (int i = 8; i < 127; i++ ) {
    //     if ( ESP_OK == i2c_master_probe( bus_handle, i, 100 ) )
    //         printf( "device found at 0x%X\n", i);
    // }
    uint32_t buffer = 0;
    // i2c_master_transmit_receive( device, (uint8_t *)"hello!!", 8, (uint8_t*) &buffer, sizeof( buffer ), -1 );
    i2c_master_transmit_receive( device, (uint8_t *)"hello!!", 8, (uint8_t*) &buffer, 2, -1 );
    printf( "%lu\n", buffer );
    vTaskDelete(NULL);  // ends itself (in a good way)
}

void app_main(void) {
    // send_get();
    static TaskHandle_t handle;
    xTaskCreate( send_get, "task", 2000, NULL, 1, &handle );
}
#else
#include "driver/spi_master.h"

// #include "freertos/task.h"

void app_main(void)
{
    spi_bus_config_t bus_config = {
        .mosi_io_num    = 4,
        .miso_io_num    = 7,
        .sclk_io_num    = 5,
        .flags          = SPICOMMON_BUSFLAG_MASTER,
    };

    ESP_ERROR_CHECK(
        spi_bus_initialize( SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO )
    );

    spi_device_interface_config_t dev_config = {
        // .command_bits   = sizeof( uint8_t ),
        // .dummy_bits     = 4,
        .mode           = 0,
        .clock_source   = SPI_CLK_SRC_DEFAULT,
        .clock_speed_hz = 10000,
        .spics_io_num   = 6,
        .queue_size     = 1,
        // .cs_ena_posttrans   = 8,    // might help arduino receive last byte idk
        // .input_delay_ns = 10000,
    };
    
    spi_device_handle_t device;
    ESP_ERROR_CHECK( 
        spi_bus_add_device( SPI2_HOST, &dev_config, &device )
    );

    char sendStr[] = "abcd&&";
    uint8_t sendData[4] = { 0x11, 0x11, 0x11, 0x11 };
    char buffer[] = "!!!!!!";

    spi_transaction_t msg = {
        // .cmd    = 2,
        .length = 4 * 8,
        // .tx_buffer  = sendStr,
        .tx_buffer  = sendData,
        .rx_buffer  = buffer,
        // .flags          = SPI_TRANS_CS_KEEP_ACTIVE, // | whatever flags
    };

    ESP_ERROR_CHECK( 
        spi_device_transmit( device, &msg )
    );

    printf( "from arduino: %s\n", buffer );
    memset( buffer, 0, sizeof( buffer ) );
}

void send_get() {

}

#endif