#include "networkConfig.h"
#include "esp_log.h" // Ensure logging is included
#include "esp_mac.h"

static const char *TAG = "wifi softAP";
// dhcps_lease_t dhcps_lease;
// static esp_err_t dhcps_lease_callback(dhcps_lease_t *lease) {
//     ESP_LOGI(TAG, "DHCP lease granted: IP=%s, MAC=" MACSTR, ip4addr_ntoa(&lease->ip), MAC2STR(lease->mac));
//     return ESP_OK;
// }
void configure_network(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS Flash needs to be erased, reinitializing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize the TCP/IP stack
    ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize TCP/IP stack: %s", esp_err_to_name(ret));
        return;
    }

    // Create default event loop
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create default event loop: %s", esp_err_to_name(ret));
        return;
    }

    // Create a default Wi-Fi access point (AP)
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();

    ip4_addr_t ip, netmask, gw;
    IP4_ADDR(&ip, 192, 168, 12, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 12, 1);
    
    esp_netif_ip_info_t ip_info = {
            .ip = { ip.addr },
            .netmask = { netmask.addr },
            .gw = { gw.addr },
    };
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(ap_netif)); // Stop DHCP server if enabled
    ESP_ERROR_CHECK(esp_netif_set_ip_info(ap_netif, &ip_info)); // Set new IP information
    ESP_ERROR_CHECK(esp_netif_dhcps_start(ap_netif)); // Restart DHCP server
    // Initialize Wi-Fi with default configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register Wi-Fi event handler: %s", esp_err_to_name(ret));
        return;
    }

    // Configure Wi-Fi with SSID and password
    wifi_config_t wifi_config = {
            .ap =
                    {
                            .ssid = CONFIG_ESP_WIFI_AP_SSID,
                            .ssid_len = strlen(CONFIG_ESP_WIFI_AP_SSID),
                            .channel = CONFIG_ESP_WIFI_AP_CHANNEL,
                            .password = CONFIG_ESP_WIFI_AP_PASSWORD,
                            .max_connection = CONFIG_ESP_MAX_STA_CONN_AP,
                            .authmode = WIFI_AUTH_WPA2_PSK,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
                            // .authmode = WIFI_AUTH_WPA3_PSK,
                            // .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else
                            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
                            .pmf_cfg =
                                    {
                                            .required = true,
                                    },
                    },
    };

    if (strlen(CONFIG_ESP_WIFI_AP_PASSWORD) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
        ESP_LOGW(TAG, "No password set, Wi-Fi AP is open!");
    }

    // Set Wi-Fi mode to access point (AP)
    ret = esp_wifi_set_mode(WIFI_MODE_AP);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi mode: %s", esp_err_to_name(ret));
        return;
    }
    ret = esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_BW_HT40);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi bandwidth: %s", esp_err_to_name(ret));
        return;
    }
    ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi configuration: %s", esp_err_to_name(ret));
        return;
    }

    // Start Wi-Fi
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start Wi-Fi: %s", esp_err_to_name(ret));
        return;
    }
    wifi_bandwidth_t bw;
    ret = esp_wifi_get_bandwidth(WIFI_IF_AP, &bw);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get Wi-Fi bandwidth: %s", esp_err_to_name(ret));
        return;
    }
    ESP_LOGI(TAG, "Wi-Fi configured successfully. SSID: %s, Channel: %d, bandwidth %d", CONFIG_ESP_WIFI_AP_SSID, CONFIG_ESP_WIFI_AP_CHANNEL, bw);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Station " MACSTR " joined, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "Station " MACSTR " left, AID=%d, reason=%d", MAC2STR(event->mac), event->aid, event->reason);
    }
}
