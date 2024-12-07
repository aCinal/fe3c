#include <fe3c/eddsa.h>
#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_event.h>
#include <esp_wifi.h>
#include <lwip/inet.h>
#include <esp_http_server.h>
#include <esp_random.h>
#include <esp_log.h>
#include <string.h>

static void setup_initial_keys(void);
static void init_wifi(void);
static void init_http_server(void);
#if CONFIG_EXAMPLE_WIFI_MODE_STA
static void tear_down_http_server(void);
static void handle_sta_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
static void handle_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
#endif /* CONFIG_EXAMPLE_WIFI_MODE_STA */
static esp_err_t put_secret_key_handler(httpd_req_t *http_req);
static esp_err_t get_public_key_handler(httpd_req_t *http_req);
static esp_err_t sign_handler(httpd_req_t *http_req);
static esp_err_t verify_handler(httpd_req_t *http_req);
static eddsa_curve get_curve_from_query(httpd_req_t *http_req);
static void bad_request(httpd_req_t *http_req);
static void unsupported_curve(httpd_req_t *http_req);
static esp_err_t read_request_body(httpd_req_t *http_req, u8 *buffer, size_t length);

static const char *TAG = "example";
static u8 *secret_keys[EDDSA_NUMBER_OF_SUPPORTED_CURVES];
static u8 *public_keys[EDDSA_NUMBER_OF_SUPPORTED_CURVES];
static httpd_handle_t http_server;
#if CONFIG_EXAMPLE_WIFI_MODE_STA
static int connect_retry_count = 0;
#endif /* CONFIG_EXAMPLE_WIFI_MODE_STA */

static const httpd_uri_t uris[] = {
    {
        .uri = "/secret",
        .method = HTTP_PUT,
        .handler = put_secret_key_handler,
        .user_ctx = NULL
    },
    {
        .uri = "/public",
        .method = HTTP_GET,
        .handler = get_public_key_handler,
        .user_ctx = NULL
    },
    {
        .uri = "/sign",
        .method = HTTP_POST,
        .handler = sign_handler,
        .user_ctx = NULL
    },
    {
        .uri = "/verify",
        .method = HTTP_POST,
        .handler = verify_handler,
        .user_ctx = NULL
    }
};

void app_main(void)
{
    /* Create initial keys for each supported curve */
    setup_initial_keys();
    /* Initialize the Wi-Fi */
    init_wifi();
#if CONFIG_EXAMPLE_WIFI_MODE_SOFTAP
    /* We are the access point - start the HTTP server synchronously */
    init_http_server();
#endif /* CONFIG_EXAMPLE_WIFI_MODE_SOFTAP */
}

static void setup_initial_keys(void)
{
    eddsa_curve curves[] = { EDDSA_CURVE_ED25519, EDDSA_CURVE_ED448 };

    for (int i = 0; i < sizeof(curves) / sizeof(curves[0]); i++) {

        eddsa_curve curve = curves[i];

        if (eddsa_is_curve_supported(curve)) {

            /* Allocate buffers for the secret and public key */
            secret_keys[curve] = malloc(eddsa_get_secret_key_length(curve));
            assert(secret_keys[curve]);
            public_keys[curve] = malloc(eddsa_get_public_key_length(curve));
            assert(public_keys[curve]);

            /* Generate a random secret key to start with */
            esp_fill_random(secret_keys[curve], eddsa_get_secret_key_length(curve));

            /* Derive the corresponding public key */
            eddsa_derive_public_key(public_keys[curve], secret_keys[curve], curve);
        }
    }
}

static void init_wifi(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

#if CONFIG_EXAMPLE_WIFI_MODE_SOFTAP

    esp_netif_create_default_wifi_ap();
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = CONFIG_EXAMPLE_WIFI_SSID,
            .ssid_len = strlen(CONFIG_EXAMPLE_WIFI_SSID),
            .channel = 1,
            .password = CONFIG_EXAMPLE_WIFI_PASS,
            .max_connection = 4,
            .authmode = strlen(CONFIG_EXAMPLE_WIFI_PASS) ? WIFI_AUTH_WPA_WPA2_PSK : WIFI_AUTH_OPEN,
            .pmf_cfg = {
                    .required = false,
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Find our IP */
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
    char ip_addr[16];
    inet_ntoa_r(ip_info.ip.addr, ip_addr, sizeof(ip_addr));
    ESP_LOGI(TAG, "Wi-Fi SoftAP initialized (SSID: %s, gateway IP: %s)", \
        CONFIG_EXAMPLE_WIFI_SSID, ip_addr);
#elif CONFIG_EXAMPLE_WIFI_MODE_STA
    esp_netif_create_default_wifi_sta();
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_EXAMPLE_WIFI_SSID,
            .password = CONFIG_EXAMPLE_WIFI_PASS,
            .scan_method = WIFI_ALL_CHANNEL_SCAN,
            .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
            .threshold.rssi = -127,
            .threshold.authmode = WIFI_AUTH_OPEN,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* Install the event listeners before we go live */
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &handle_sta_got_ip, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &handle_wifi_disconnect, NULL));

    ESP_ERROR_CHECK(esp_wifi_connect());
#else
    #error "Neither CONFIG_EXAMPLE_WIFI_MODE_SOFTAP nor CONFIG_EXAMPLE_WIFI_MODE_STA defined"
#endif
}

static void init_http_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting the signing oracle server on port %d...", \
        config.server_port);
    ESP_ERROR_CHECK(httpd_start(&http_server, &config));

    for (int i = 0; i < sizeof(uris) / sizeof(uris[0]); i++)
        httpd_register_uri_handler(http_server, &uris[i]);
}

#if CONFIG_EXAMPLE_WIFI_MODE_STA
static void tear_down_http_server(void)
{
    ESP_ERROR_CHECK(httpd_stop(http_server));
}

static void handle_sta_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    (void) arg;
    (void) event_base;
    (void) event_id;

    /* Successfully connected, clear the retry counter */
    connect_retry_count = 0;

    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;

    ESP_LOGI(TAG, "Assigned IP address: " IPSTR " for net interface \"%s\"", \
        IP2STR(&event->ip_info.ip), esp_netif_get_desc(event->esp_netif));

    /* Start the server */
    init_http_server();
}

static void handle_wifi_disconnect(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    (void) arg;
    (void) event_base;
    (void) event_id;
    (void) event_data;

    /* Tear down the server */
    tear_down_http_server();

    if (connect_retry_count < CONFIG_EXAMPLE_WIFI_MAX_CONNECT_RETRIES) {

        ESP_LOGW(TAG, "Wi-Fi disconnected. Trying to reconnect...");
        ESP_ERROR_CHECK(esp_wifi_connect());
        connect_retry_count++;

    } else {

        ESP_LOGW(TAG, "Failed to connect to the access point %d time(s). Not retrying anymore...", \
            connect_retry_count);
    }
}
#endif /* CONFIG_EXAMPLE_WIFI_MODE_STA */

static esp_err_t put_secret_key_handler(httpd_req_t *http_req)
{
    ESP_LOGD(TAG, "Handling put secret key request...");
    eddsa_curve curve = get_curve_from_query(http_req);

    if (EDDSA_NUMBER_OF_SUPPORTED_CURVES == curve) {

        /* Malformed request */
        bad_request(http_req);
        return ESP_OK;
    }

    /* Check that the curve is supported */
    if (!eddsa_is_curve_supported(curve)) {

        /* Unsupported curve */
        ESP_LOGD(TAG, "Valid, but unsuppored curve: %d", curve);
        unsupported_curve(http_req);
        return ESP_OK;
    }

    /* Check that the request body is long enough to contain the secret key */
    if (http_req->content_len < eddsa_get_secret_key_length(curve)) {

        /* Malformed request */
        bad_request(http_req);
        ESP_LOGW(TAG, \
            "Malformed secret key request (PUT). Content length: %d, expected: %d (secret key length for curve: %d)", \
            http_req->content_len, eddsa_get_secret_key_length(curve), curve);
        return ESP_OK;
    }

    if (http_req->content_len > eddsa_get_secret_key_length(curve)) {

        ESP_LOGW(TAG, \
            "Request body length (%d) is larger than the length of the secret key (%d) for curve %d. Ignoring %d excess bytes...", \
            http_req->content_len, eddsa_get_secret_key_length(curve), curve, http_req->content_len - eddsa_get_secret_key_length(curve));
    }

    /* Read the secret key */
    if (unlikely(ESP_OK != read_request_body(http_req, secret_keys[curve], eddsa_get_secret_key_length(curve)))) {

        /* In case of error, returning ESP_FAIL will ensure that the underlying
         * socket is closed */
        return ESP_FAIL;
    }

    ESP_LOGD(TAG, "New secret key set. Deriving the corresponding public key...");
    /* Derive the corresponding public key */
    eddsa_derive_public_key(public_keys[curve], secret_keys[curve], curve);

    /* Respond with 200 OK */
    httpd_resp_send(http_req, NULL, 0);

    return ESP_OK;
}

static esp_err_t get_public_key_handler(httpd_req_t *http_req)
{
    ESP_LOGD(TAG, "Handling get public key request...");
    eddsa_curve curve = get_curve_from_query(http_req);

    if (EDDSA_NUMBER_OF_SUPPORTED_CURVES == curve) {

        /* Malformed request */
        bad_request(http_req);
        return ESP_OK;
    }

    /* Check that the curve is supported */
    if (!eddsa_is_curve_supported(curve)) {

        /* Unsupported curve */
        ESP_LOGD(TAG, "Valid, but unsuppored curve: %d", curve);
        unsupported_curve(http_req);
        return ESP_OK;
    }

    /* Respond with the public key */
    ESP_ERROR_CHECK(httpd_resp_set_type(http_req, "application/octet-stream"));
    httpd_resp_send(http_req, (char *) public_keys[curve], eddsa_get_public_key_length(curve));

    return ESP_OK;
}

static esp_err_t sign_handler(httpd_req_t *http_req)
{
    ESP_LOGD(TAG, "Handling sign request...");
    eddsa_curve curve = get_curve_from_query(http_req);

    if (EDDSA_NUMBER_OF_SUPPORTED_CURVES == curve) {

        /* Malformed request */
        bad_request(http_req);
        return ESP_OK;
    }

    /* Check that the curve is supported */
    if (!eddsa_is_curve_supported(curve)) {

        /* Unsupported curve */
        ESP_LOGD(TAG, "Valid, but unsuppored curve: %d", curve);
        unsupported_curve(http_req);
        return ESP_OK;
    }

    u8 *message = NULL;
    if (http_req->content_len > 0) {

        message = malloc(http_req->content_len);
        assert(message);

        /* Read the message */
        if (unlikely(ESP_OK != read_request_body(http_req, message, http_req->content_len))) {

            free(message);
            /* In case of error, returning ESP_FAIL will ensure that the underlying
             * socket is closed */
            return ESP_FAIL;
        }
    }

    /* Allocate the signature buffer on the heap to not strain the stack capacity */
    u8 *signature = malloc(eddsa_get_signature_length(curve));
    assert(signature);

    eddsa_sign_request sign_req = {
        .signature = signature,
        .secret_key = secret_keys[curve],
        .public_key = public_keys[curve],
        .message = message,
        .message_length = http_req->content_len,
        .curve_id = curve
    };
    ESP_LOGD(TAG, "Signing message of length %d on curve %d...", \
        sign_req.message_length, sign_req.curve_id);
    eddsa_sign(&sign_req);
    /* Release the message buffer */
    free(message);

    /* Respond with the signature */
    ESP_ERROR_CHECK(httpd_resp_set_type(http_req, "application/octet-stream"));
    httpd_resp_send(http_req, (char *) signature, eddsa_get_signature_length(curve));
    /* Free the signature buffer */
    free(signature);

    return ESP_OK;
}

static esp_err_t verify_handler(httpd_req_t *http_req)
{
    ESP_LOGD(TAG, "Handling verify request...");
    eddsa_curve curve = get_curve_from_query(http_req);

    if (EDDSA_NUMBER_OF_SUPPORTED_CURVES == curve) {

        /* Malformed request */
        bad_request(http_req);
        return ESP_OK;
    }

    /* Check that the curve is supported */
    if (!eddsa_is_curve_supported(curve)) {

        /* Unsupported curve */
        ESP_LOGD(TAG, "Valid, but unsuppored curve: %d", curve);
        unsupported_curve(http_req);
        return ESP_OK;
    }

    /* Assume format (signature, message). Check that the request body is long enough
     * to contain at least the signature. */
    if (http_req->content_len < eddsa_get_signature_length(curve)) {

        /* Malformed request */
        bad_request(http_req);
        ESP_LOGW(TAG, \
            "Malformed verification request (POST). Content length: %d, expected: %d (signature length for curve: %d)", \
            http_req->content_len, eddsa_get_signature_length(curve), curve);
        return ESP_OK;
    }

    u8 *payload = malloc(http_req->content_len);
    assert(payload);
    /* Read the signature and the message in one go */
    if (unlikely(ESP_OK != read_request_body(http_req, payload, http_req->content_len))) {

        free(payload);
        /* In case of error, returning ESP_FAIL will ensure that the underlying
         * socket is closed */
        return ESP_FAIL;
    }

    eddsa_verify_request verify_req = {
        .signature = payload,
        .message = payload + eddsa_get_signature_length(curve),
        .message_length = http_req->content_len - eddsa_get_signature_length(curve),
        .public_key = public_keys[curve],
        .curve_id = curve
    };
    ESP_LOGD(TAG, "Verifying a signature on message of length %d on curve %d...", \
        verify_req.message_length, verify_req.curve_id);
    int signature_ok = eddsa_verify(&verify_req);

    httpd_resp_send(http_req, signature_ok ? "Signature ok" : "Signature invalid", HTTPD_RESP_USE_STRLEN);
    free(payload);

    return ESP_OK;
}

static eddsa_curve get_curve_from_query(httpd_req_t *http_req)
{
    /* Look for the query parameter denoting the curve */
    size_t query_len = httpd_req_get_url_query_len(http_req);
    if (query_len == 0) {

        /* Malformed request */
        ESP_LOGW(TAG, "Malformed request. No query string.");
        return EDDSA_NUMBER_OF_SUPPORTED_CURVES;
    }

    /* Get the query string */
    char *query = malloc(query_len + 1);
    assert(query);
    ESP_ERROR_CHECK(httpd_req_get_url_query_str(http_req, query, query_len + 1));

    char curve_name[32];
    /* Find the curve name in the query string */
    if (ESP_OK != httpd_query_key_value(query, "curve", curve_name, sizeof(curve_name))) {

        /* Malformed request */
        free(query);
        ESP_LOGW(TAG, "Malformed request. Curve not specified in query string.");
        return EDDSA_NUMBER_OF_SUPPORTED_CURVES;
    }

    /* We no longer need the query string, free the buffer */
    free(query);

    eddsa_curve curve;
    /* Check the curve parameter is correct */
    if (0 == strcmp(curve_name, "ed25519")) {

        curve = EDDSA_CURVE_ED25519;

    } else if (0 == strcmp(curve_name, "ed448")) {

        curve = EDDSA_CURVE_ED448;

    } else {

        /* Malformed request */
        ESP_LOGW(TAG, "Malformed request. Invalid curve name: %s", curve_name);
        return EDDSA_NUMBER_OF_SUPPORTED_CURVES;
    }

    return curve;
}

static void bad_request(httpd_req_t *http_req)
{
    ESP_ERROR_CHECK(httpd_resp_set_status(http_req, HTTPD_400));
    httpd_resp_send(http_req, NULL, 0);
}

static void unsupported_curve(httpd_req_t *http_req)
{
    ESP_ERROR_CHECK(httpd_resp_set_status(http_req, HTTPD_404));
    httpd_resp_send(http_req, "Unsupported curve", HTTPD_RESP_USE_STRLEN);
}

static esp_err_t read_request_body(httpd_req_t *http_req, u8 *buffer, size_t length)
{
    while (length > 0) {

        int received = httpd_req_recv(http_req, (char *) buffer, length);
        if (unlikely(received <= 0)) {

            if (received == HTTPD_SOCK_ERR_TIMEOUT)
                httpd_resp_send_408(http_req);
            /* In case of error, returning ESP_FAIL will ensure that the underlying
             * socket is closed */
            return ESP_FAIL;
        }

        /* Retry if failed to receive everything in one go */
        buffer += received;
        length -= received;
    }

    return ESP_OK;
}
