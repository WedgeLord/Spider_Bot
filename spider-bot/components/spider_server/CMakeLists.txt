set( requires
    # required components listed here
    esp_wifi 
    esp_http_server 
    nvs_flash 
    spiffs 
)

idf_component_register(SRCS "spider_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES ${requires})

# this is how the website data is stored on the esp32
spiffs_create_partition_image(storage "./website" FLASH_IN_PROJECT)