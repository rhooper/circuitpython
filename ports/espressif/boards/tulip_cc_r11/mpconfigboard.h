//
// Created by Rose Hooper on 10/30/24.
//

#pragma once

#define MICROPY_HW_BOARD_NAME       "Tulip CC R11"
#define MICROPY_HW_MCU_NAME         "ESP32S3"

// UART pins attached to the USB-serial converter chip
#define CIRCUITPY_CONSOLE_UART_TX (&pin_GPIO43)
#define CIRCUITPY_CONSOLE_UART_RX (&pin_GPIO44)

//#define CONFIG_I2S_LRCLK 2
//#define CONFIG_I2S_BCLK 8
#define DEFAULT_I2C_BUS_SDA (&pin_GPIO17)
#define DEFAULT_I2C_BUS_SCL (&pin_GPIO18)


//// Set all but G2 and B3 off for R10 (i2s using G2, B3)
//#ifdef TULIP4_R11
//set_pin(PIN_B4, 0);
//    set_pin(PIN_B5, 0);
//    set_pin(PIN_G3, 0);
//    set_pin(PIN_G4, 0);
//    set_pin(PIN_R3, 0);
//    set_pin(PIN_R4, 0);
//#endif
