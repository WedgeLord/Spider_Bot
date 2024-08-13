#include <stdio.h>
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "esp_spiffs.h"

#include "spider_server.h"

#define WIFI_SSID "spiderbot"
#define WIFI_PSWD "spiderbot"
#define WIFI_CHAN 1

#define MDNS_ADDR esp_ip4addr_aton( "224.0.0.251" ) //0xFF02FB  // set according to android mDNS docs

#define PARAM_SIZE 4

static const char *TAG_AP = "ESP32 Access Point";
static const char *TAG_DNS = "mDNS Server";

esp_err_t setMotorHandler( httpd_req_t * );
esp_err_t catchAllHandler( httpd_req_t * );
void prepareWebpage();

char webpage[4000];
int webpage_size = 0;

void startSpiderServer( spider_leg_t *spider_leg ) {
// void startSpiderServer( void *user_callback( spider_leg_t ) ) {
    ESP_ERROR_CHECK(
        nvs_flash_init()
    );

    ESP_ERROR_CHECK(
        esp_netif_init()
    );

    ESP_ERROR_CHECK(
        esp_event_loop_create_default()
    );

    wifi_init_config_t wifi_init = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(
        esp_wifi_init( &wifi_init )
    );

    ESP_ERROR_CHECK( 
        esp_wifi_set_mode( WIFI_MODE_AP )
    );

    esp_netif_t *netif_ap = esp_netif_create_default_wifi_ap();
    /*
    */

    wifi_config_t wifi_config = {
        .ap = {
            .ssid       = WIFI_SSID,
            .password   = WIFI_PSWD,
            .channel    = WIFI_CHAN,
            .authmode   = WIFI_AUTH_OPEN,
            .max_connection     = 4,  // no more than 1 controller allowed
            .pmf_cfg.required   = false
        }
    };

    ESP_ERROR_CHECK(
        esp_wifi_set_config( WIFI_IF_AP, &wifi_config )
    );

    ESP_LOGI(TAG_AP, 
    "wifi_init_softap finished. SSID:%s password:%s channel:%d", WIFI_SSID, WIFI_PSWD, WIFI_CHAN);

    ESP_ERROR_CHECK(
        esp_wifi_start() 
    );

    httpd_handle_t server = NULL;
    httpd_config_t server_config = HTTPD_DEFAULT_CONFIG();
    server_config.lru_purge_enable = true;

    ESP_LOGI( TAG_AP, 
        "Starting server on port: %d", server_config.server_port );

    const httpd_uri_t uri_setMotor =  {
        .uri        = "/setMotor",
        .method     = HTTP_GET,
        .handler    = setMotorHandler,
        .user_ctx   = spider_leg
    };
    const httpd_uri_t uri_catchAll = {
        .uri        = "/",
        .method     = HTTP_GET,
        .handler    = catchAllHandler,
        .user_ctx   = NULL
    };
    if ( httpd_start( &server, &server_config ) == ESP_OK ) {
        httpd_register_uri_handler( server, &uri_setMotor );
        httpd_register_uri_handler( server, &uri_catchAll );
        // return server;
    }
    else {
        ESP_LOGI( TAG_AP, 
            "Error starting server :(" );
        // return NULL;
    }

    prepareWebpage();
}

esp_err_t setMotorHandler( httpd_req_t *req ) {
    int leg, height, pivot;
    char buffer[64] = {0};
    esp_err_t err = ESP_OK;

    // assuming length is never more than 63
    httpd_req_get_hdr_value_str( req, "Host", buffer, 64 );
    // httpd_req_get_hdr_value_str( req, "Host", buffer, httpd_req_get_hdr_value_len( req, "Host" ) + 1 );

    ESP_LOGI( TAG_AP,
        "Host '%s' made request", buffer );

    memset( buffer, 0, 64 );
    ESP_ERROR_CHECK_WITHOUT_ABORT( 
        err = httpd_req_get_url_query_str( req, buffer, 64 )
    );
    ESP_LOGI( TAG_AP,
        "URL query: '%s'", buffer );
    char param[3][PARAM_SIZE] = {0};
    err |=  httpd_query_key_value( buffer, "leg", param[1], PARAM_SIZE );
    err |= httpd_query_key_value( buffer, "height", param[2], PARAM_SIZE );
    err |= httpd_query_key_value( buffer, "pivot", param[3], PARAM_SIZE );
    ESP_ERROR_CHECK_WITHOUT_ABORT( err );
    if ( err ) {
        return err;
    }
    leg = strtod( param[1], NULL );
    height = strtod( param[2], NULL );
    pivot = strtod( param[3], NULL );
    ESP_LOGI( TAG_AP,
        "query parameters: leg: %d, height: %d, pivot: %d", leg, height, pivot );
    if ( 
        ( leg < 1 || leg > 4 ) 
        || ( height < 0 || height > 180 )
        || ( pivot < 0 || pivot > 180 )
    ) {
        ESP_LOGD( TAG_AP,
            "Invalid query parameters, command rejected" );
    }
    spider_leg_t *user_spider_leg = req->user_ctx;
    user_spider_leg->leg = leg;
    user_spider_leg->height = height;
    user_spider_leg->pivot = pivot;
    /*
    spider_leg_t user_spider_leg = { leg, height, pivot };
    
    req->user_ctx( user_spider_leg );
    */

    return err;
}

esp_err_t catchAllHandler( httpd_req_t * req ) {
    char buffer[] = {0};
    esp_err_t err;

    ESP_ERROR_CHECK_WITHOUT_ABORT(
        httpd_req_get_url_query_str( req, buffer, 64 )
    );

    ESP_LOGI( TAG_AP,
        "request received: '%s', sending webpage...", buffer
    );

    ESP_ERROR_CHECK_WITHOUT_ABORT( 
        httpd_resp_send( req, webpage, webpage_size )
    );
    return ESP_OK;
}

void prepareWebpage() {
    ESP_LOGI( TAG_AP,
        "Storing webpage in buffer");
    esp_vfs_spiffs_conf_t spiffs_config = {
        .base_path  = "/spiffs",
        .partition_label    = NULL,
        .max_files  = 5,
    };

    esp_err_t err = esp_vfs_spiffs_register( &spiffs_config );
    if ( err == ESP_ERR_NOT_FOUND ) {
        ESP_LOGE( TAG_AP,
            "Partition not found, have you enabled custom partions AND used the partition.csv file in the component folder?" );
    }
    else {
        ESP_ERROR_CHECK( err );
    }

    FILE *f_html = fopen("/spiffs/spiderbot.html", "r");
    ESP_LOGI( TAG_AP,
        "%s", (f_html != NULL) ? "File found" : "File not found!");
    while ( fgets( webpage + webpage_size, 4000 - webpage_size, f_html ) ) {
        webpage_size += strlen( webpage + webpage_size );
    }
    fclose( f_html );
    ESP_LOGI( TAG_AP, 
        "File stored (%.2f kB)", webpage_size / 1024.0 );
}
