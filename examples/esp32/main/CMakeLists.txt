set(DEPENDENCIES
    nvs_flash
    esp_netif
    esp_wifi
    esp_http_server
    cementite
)

idf_component_register(SRCS "example.c"
                       PRIV_REQUIRES ${DEPENDENCIES})
