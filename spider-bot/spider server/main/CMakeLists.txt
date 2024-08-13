idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

# this is how the website data is stored on the esp32
spiffs_create_partition_image(storage "../website" FLASH_IN_PROJECT)