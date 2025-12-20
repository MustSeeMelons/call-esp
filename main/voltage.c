#include "include/voltage.h"
#include "adc_cali_schemes.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

#define VOLTAGE_DIVIDER_FACTOR 2.0f
#define ADC_CHANNEL            ADC_CHANNEL_6

static adc_oneshot_unit_handle_t adc_handle = NULL;
static adc_cali_handle_t cali_handle = NULL;

static adc_cali_handle_t adc_cali_create(adc_unit_t unit, adc_atten_t atten, adc_bitwidth_t width) {
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = ESP_FAIL;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = width,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit,
        .atten = atten,
        .bitwidth = width,
    };
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
#else
#error "No supported ADC calibration scheme found"
#endif

    if (ret == ESP_OK) {
        printf("ADC calibration initialized successfully\n");
    } else {
        printf("ADC calibration not supported, using raw readings\n");
        handle = NULL;
    }

    return handle;
}

esp_err_t voltage_init() {
    esp_err_t err = ESP_OK;

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_1,
    };

    err = adc_oneshot_new_unit(&unit_cfg, &adc_handle);

    if (err != ESP_OK) {
        return err;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12,
    };

    // GPIO34
    err = adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &chan_cfg);

    if (err != ESP_OK) {
        return err;
    }

    cali_handle = adc_cali_create(ADC_UNIT_1, chan_cfg.atten, chan_cfg.bitwidth);

    return err;
}

float read_battery_voltage(void) {
    int raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &raw));

    int voltage_mv = 0;
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(cali_handle, raw, &voltage_mv));

    float vbat = (voltage_mv / 1000.0f) * VOLTAGE_DIVIDER_FACTOR;
    return vbat;
}