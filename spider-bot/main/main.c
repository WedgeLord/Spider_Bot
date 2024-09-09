#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "spider_server.h"

#ifdef I2C
#include "driver/i2c_master.h"
i2c_master_dev_handle_t device;

void start_communication( uint8_t *command ) {
    //setup 
    i2c_master_bus_config_t bus_config = {
        .i2c_port       = -1,
        .sda_io_num     = 4,
        .scl_io_num     = 5,
        .clk_source     = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        // used if you don't have physical external pull-up resistors. System might not work using just internal pullups
        .flags.enable_internal_pullup = false    // ".flags" missing from IDF docs :(
    };

    i2c_master_bus_handle_t bus_handle; 
    ESP_ERROR_CHECK(
        i2c_new_master_bus( &bus_config, &bus_handle )
    );

    i2c_device_config_t dev_config = {
        .dev_addr_length    = I2C_ADDR_BIT_LEN_7,
        .device_address     = 0x55,
        .scl_speed_hz       = 30000,
    };

    // i2c_master_dev_handle_t device;
    ESP_ERROR_CHECK(
        i2c_master_bus_add_device( bus_handle, &dev_config, &device )
    );

    // for (int i = 8; i < 127; i++ ) {
    //     if ( ESP_OK == i2c_master_probe( bus_handle, i, 100 ) )
    //         printf( "device found at 0x%X\n", i);
    // }
    
    command[0] = 1;
    //REMOVE
    spider_server_callback( (tie_params){ { 1, 2, 0, 0 } } );
    while (1) { // send updates to robot
    /*
        // ESP_ERROR_CHECK_WITHOUT_ABORT(
        printf( "spider=[%d][%d][%d]\n", spider_leg->leg, spider_leg->height, spider_leg->pivot );
            i2c_master_transmit( device, command, sizeof( spider_leg_t + 1 ), -1);
        // );
    */
        vTaskDelay( pdMS_TO_TICKS( 500 ) ); // wait 100ms
    }
}
void send( uint8_t command, uint8_t *send_buffer, uint8_t *rec_buffer ) {
    size_t send_size, rec_size;
    // unfinished 
    switch ( command ) {
        case 2: 
            send_size = sizeof( tie_params );
            rec_size = sizeof( tie_return )
        
        default:
            return;
    }
    printf("sending\n");
    ESP_ERROR_CHECK_WITHOUT_ABORT(
        i2c_master_transmit( device, send_buffer, send_size, -1 )
    );
    printf("receiving\n");
    i2c_master_receive( device, rec_buffer, rec_size, -1 );
}

#else
#include "driver/spi_master.h"

spi_device_handle_t device;

void start_communication() {
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
        .mode           = 0,
        .clock_source   = SPI_CLK_SRC_DEFAULT,
        .clock_speed_hz = 10000,
        .spics_io_num   = 6,
        .queue_size     = 1,
    };
    
    ESP_ERROR_CHECK( 
        spi_bus_add_device( SPI2_HOST, &dev_config, &device )
    );

    vTaskDelete( NULL );
}

void send( uint8_t command, uint8_t *send_buffer, uint8_t *rec_buffer ) {
    size_t send_size, rec_size;

    switch ( command ) {
        case 1:
            send_size = sizeof( spider_leg_t );
            break;
        
        case 2:
            send_size = sizeof( tie_params );
            rec_size  = sizeof( tie_return );
            break;
        
        default:
            printf( "INVALID COMMAND\n" );
            return;
    }
    
    // send phase uses command bits, so the _ext struct is used to change the default cmd bits
    spi_transaction_ext_t send_phase = {
        .base = {
            .cmd        = command,
            .length     = send_size * 8,
            .tx_buffer  = send_buffer,
            .flags      = SPI_TRANS_VARIABLE_CMD
        },
        .command_bits   = 8
    };

    ESP_ERROR_CHECK( 
        spi_device_transmit( device, &send_phase )
    );

    spi_transaction_t rec_phase = {
        .length = rec_size * 8,
        .rx_buffer = rec_buffer
    };

    ESP_ERROR_CHECK( 
        spi_device_transmit( device, &rec_phase )
    );
}

#endif

void spider_server_callback( tie_params );
void app_main(void)
{
    static uint8_t command[sizeof(spider_leg_t) + 1];
    TaskHandle_t comm_task;
    xTaskCreate( start_communication, "COMM TASK", 4000, command, 1, &comm_task);
    configASSERT( comm_task );
    spider_server_callback( (tie_params){{ 0x11, 0x22, 0x44, 0x88 }} );
    // startSpiderServer( (spider_leg_t *) (command + 1), spider_server_callback ); 
}
void spider_server_callback( tie_params params ) {
    puts( "Callback triggered!" );
    tie_return leg_bounds = {0};
    send( 2, &params, &leg_bounds );
    printf( "bounds received: low[%d] high[%d] || low[%d] high[%d]\n", 
        leg_bounds.leg1.lower, leg_bounds.leg1.upper, leg_bounds.leg2.lower, leg_bounds.leg2.upper );
}