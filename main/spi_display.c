#include "include/spi_display.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "stdint.h"

#define MOSI 23
#define CLK  18
#define RCK  4 // Latch

static spi_device_handle_t spi_handle;

static esp_err_t init_latch() {
    esp_err_t err;

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << RCK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    err = gpio_config(&io_conf);

    return err;
}

esp_err_t init_spi_display() {
    esp_err_t err;

    spi_bus_config_t buscfg = {
        .mosi_io_num = MOSI,
        .sclk_io_num = CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };

    err = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1,
        .flags = 0,
    };

    err = init_latch();

    if (err != ESP_OK) {
        return err;
    }

    return spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
}

void do_display() {
    gpio_set_level(RCK, 0);

    uint8_t tx[2] = {1, 1};

    spi_transaction_t t = {
        .length = 2,
        .tx_buffer = tx,
    };

    spi_device_transmit(spi_handle, &t);

    gpio_set_level(RCK, 1);
}