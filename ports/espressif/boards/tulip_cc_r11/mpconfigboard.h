//
// Created by Rose Hooper on 10/30/24.
//

#pragma once

#define MICROPY_HW_BOARD_NAME       "Tulip CC R11"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO43)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO44)
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = &pin_GPIO43, .rx = &pin_GPIO44}}

#define CIRCUITPY_BOARD_I2C         (1)
#define CIRCUITPY_BOARD_I2C_PIN     {{.scl = &pin_GPIO18, .sda = &pin_GPIO17}}

#define DOUBLE_TAP_PIN              (&pin_GPIO0)
//#define CONFIG_I2S_LRCLK 2
//#define CONFIG_I2S_BCLK 8

#define TULIP_DISPLAY_FREQUENCY 16000000
#define TULIP_DISPLAY_WIDTH     1024
#define TULIP_DISPLAY_HEIGHT    600

#define TULIP_DISPLAY_HSYNC_PULSE_WIDTH  30
#define TULIP_DISPLAY_HSYNC_FRONT_PORCH  210
#define TULIP_DISPLAY_HSYNC_BACK_PORCH   16
#define TULIP_DISPLAY_HSYNC_IDLE_LOW     false
#define TULIP_DISPLAY_VSYNC_PULSE_WIDTH  13
#define TULIP_DISPLAY_VSYNC_FRONT_PORCH  22
#define TULIP_DISPLAY_VSYNC_BACK_PORCH   10
#define TULIP_DISPLAY_VSYNC_IDLE_LOW     false
#define TULIP_DISPLAY_DE_IDLE_HIGH       false
#define TULIP_DISPLAY_PCLK_ACTIVE_HIGH   false
#define TULIP_DISPLAY_PCLK_IDLE_HIGH     false
