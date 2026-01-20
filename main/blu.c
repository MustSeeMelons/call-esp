#include "include/blu.h"
#include "host/ble_hs.h"
#include "include/sim800l.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *TAG = "BLE";

static const uint16_t COMPANY_ID = 0x2A00;
static const uint16_t AD_DELAY = 6000; // Our button has a 5 sec ad interval
static TickType_t last_time = 0;
static uint32_t payload_gate = 0x45;
static uint32_t payload_cool = 0x4545;

static void ble_host_task(void *param) {
    nimble_port_run();
    nimble_port_freertos_deinit(); // If previous call ever returns, which it should not
}

// Generic Access Profle - we are an observer
static int ble_gap_event_cb(struct ble_gap_event *event, void *arg) {

    TickType_t now = xTaskGetTickCount();

    // Let there be sa small idle gap
    if ((now - last_time) < pdMS_TO_TICKS(AD_DELAY)) {
        return ESP_OK;
    }

    switch (event->type) {
    case BLE_GAP_EVENT_DISC: {
        struct ble_hs_adv_fields fields;
        int err = ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);

        if (err != ESP_OK) {
            return err;
        }

        if (fields.mfg_data && fields.mfg_data_len >= 2) {

            // First two bytes are the company
            uint16_t company_id = fields.mfg_data[0] | (fields.mfg_data[1] << 8);

            if (company_id == COMPANY_ID) {
                const uint32_t payload = fields.mfg_data[2] << (8 * 2) | fields.mfg_data[3] << 8 | fields.mfg_data[4];

                if (payload == payload_gate) {
                    ESP_LOGI(TAG, "Open gate");
                    sim800l_call_first();
                } else if (payload == payload_cool) {
                    ESP_LOGI(TAG, "Open cool gate");
                    sim800l_call_other();
                }

                last_time = xTaskGetTickCount();
            }
        }
        break;
    }
    default:
        ESP_LOGI(TAG, "Unknown GAP event, %d", event->type);
        break;
    }

    return ESP_OK;
}

static void ble_start_scanning(void) {
    struct ble_gap_disc_params scan_params = {.itvl = 0x50,   // 50ms
                                              .window = 0x30, // 30ms
                                              .passive = 1,
                                              .filter_duplicates = 0,                   // Payload is ignored, can't use
                                              .filter_policy = BLE_HCI_SCAN_FILT_NO_WL, // Perhaps add to whitelist?
                                              .limited = 0};

    int rc = ble_gap_disc(BLE_OWN_ADDR_PUBLIC, BLE_HS_FOREVER, &scan_params, ble_gap_event_cb, NULL);

    if (rc != 0) {
        ESP_LOGE(TAG, "ble_gap_disc failed; rc=%d", rc);
    } else {
        ESP_LOGI(TAG, "Scanning started");
    }
}

static void blu_on_sync(void) {
    ESP_LOGI(TAG, "Sync");
    ble_start_scanning();
}

esp_err_t blu_init() {
    esp_err_t err = ESP_OK;

    err = nimble_port_init();

    if (err != ESP_OK) {
        return err;
    }

    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_hs_cfg.sync_cb = blu_on_sync;

    nimble_port_freertos_init(ble_host_task);

    return err;
}