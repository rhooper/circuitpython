
#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"

//
//
//// pins.h
//// pins (and other MCU constants) for all Tulip variants
//
//#define BK_LIGHT_ON_LEVEL  1
//#ifndef DEFAULT_BRIGHTNESS
//#define DEFAULT_BRIGHTNESS 9 // 9 is max, 1 is min
//#endif
//#define MAKERFABS 1
//#define TULIP4_R11 1
//#define BK_LIGHT_OFF_LEVEL !BK_LIGHT_ON_LEVEL
//#define SPI_LL_DATA_MAX_BIT_LEN (1 << 18)
//
//#define BK_LIGHT_ON_LEVEL  1
//#ifndef DEFAULT_BRIGHTNESS
//#define DEFAULT_BRIGHTNESS 9 // 9 is max, 1 is min
//#endif
//#define BK_LIGHT_OFF_LEVEL !BK_LIGHT_ON_LEVEL
//#define SPI_LL_DATA_MAX_BIT_LEN (1 << 18)
//
//#define I2C_SDA 17
//#define I2C_SCL 18
//#define TOUCH_INT -1
//#define I2C_NUM I2C_NUM_0
//#define I2C_CLK_FREQ 100000
//#define DEFAULT_BRIGHTNESS 9 // 9 is max, 1 is min
//#define HSYNC_BACK_PORCH 16
//#define HSYNC_FRONT_PORCH 210
//#define HSYNC_PULSE_WIDTH 30
//#define VSYNC_BACK_PORCH 10
//#define VSYNC_FRONT_PORCH 22
//#define VSYNC_PULSE_WIDTH 13
//#define PIN_NUM_HSYNC          39
//#define PIN_NUM_VSYNC          41
//#define PIN_NUM_DE             40
//#define PIN_NUM_PCLK           42
//
//#define TOUCH_RST 48
//#define PIN_NUM_BK_PWM 47
//#define PIN_R7 14
//#define PIN_R6 21
//#define PIN_R5 10
//#define PIN_R4 13
//#define PIN_R3 45
//#define PIN_G7 4
//#define PIN_G6 16
//#define PIN_G5 15
//#define PIN_G4 7
//#define PIN_G3 6
//#define PIN_G2 -1
//#define PIN_B7 1
//#define PIN_B6 9
//#define PIN_B5 46
//#define PIN_B4 38
//#define PIN_B3 -1
//#define ESP_INTR_FLAG_DEFAULT 0
//#define CONFIG_I2S_LRCLK 2
//#define CONFIG_I2S_BCLK 8
//#define CONFIG_I2S_DIN 5
//#define CONFIG_I2S_NUM 0
//#define MIDI_OUT_PIN 11
//#define MIDI_IN_PIN 12
//
//#define PIN_NUM_DISP_EN        -1

// Audio: DIN IO5; BCLK IO8; LRC IO2

static const mp_rom_obj_tuple_t tft_r_pins = {
        {&mp_type_tuple},
        5,
        {
                MP_ROM_PTR(&pin_GPIO45), // R3
                MP_ROM_PTR(&pin_GPIO13), // R4
                MP_ROM_PTR(&pin_GPIO10), // R5
                MP_ROM_PTR(&pin_GPIO21), // R6
                MP_ROM_PTR(&pin_GPIO14), // R7
        }
};

static const mp_rom_obj_tuple_t tft_g_pins = {
        {&mp_type_tuple},
        5,
        {
                MP_ROM_PTR(&pin_GPIO6),
                MP_ROM_PTR(&pin_GPIO7),
                MP_ROM_PTR(&pin_GPIO15),
                MP_ROM_PTR(&pin_GPIO16),
                MP_ROM_PTR(&pin_GPIO4),
        }
};

static const mp_rom_obj_tuple_t tft_b_pins = {
        {&mp_type_tuple},
        4,
        {
                MP_ROM_PTR(&pin_GPIO38),
                MP_ROM_PTR(&pin_GPIO46),
                MP_ROM_PTR(&pin_GPIO9),
                MP_ROM_PTR(&pin_GPIO1),
        }
};

static const mp_rom_map_elem_t tft_pins_table[] = {
        { MP_ROM_QSTR(MP_QSTR_de), MP_ROM_PTR(&pin_GPIO40) },
        { MP_ROM_QSTR(MP_QSTR_vsync), MP_ROM_PTR(&pin_GPIO41) },
        { MP_ROM_QSTR(MP_QSTR_hsync), MP_ROM_PTR(&pin_GPIO40) },
        { MP_ROM_QSTR(MP_QSTR_dclk), MP_ROM_PTR(&pin_GPIO42) },
        { MP_ROM_QSTR(MP_QSTR_red), MP_ROM_PTR(&tft_r_pins) },
        { MP_ROM_QSTR(MP_QSTR_green), MP_ROM_PTR(&tft_g_pins) },
        { MP_ROM_QSTR(MP_QSTR_blue), MP_ROM_PTR(&tft_b_pins) },
};
MP_DEFINE_CONST_DICT(tft_pins_dict, tft_pins_table);

static const mp_rom_map_elem_t timings1024_table[] = {
        { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_INT(10000000) }, // nominal 16MHz, but display is unstable/tears at that frequency
        { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_INT(1024) },
        { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_INT(600) },
        { MP_ROM_QSTR(MP_QSTR_hsync_pulse_width), MP_ROM_INT(30) },
        { MP_ROM_QSTR(MP_QSTR_hsync_front_porch), MP_ROM_INT(210) },
        { MP_ROM_QSTR(MP_QSTR_hsync_back_porch), MP_ROM_INT(16) },
        { MP_ROM_QSTR(MP_QSTR_hsync_idle_low), MP_ROM_FALSE },
        { MP_ROM_QSTR(MP_QSTR_vsync_pulse_width), MP_ROM_INT(13) },
        { MP_ROM_QSTR(MP_QSTR_vsync_front_porch), MP_ROM_INT(22) },
        { MP_ROM_QSTR(MP_QSTR_vsync_back_porch), MP_ROM_INT(10) },
        { MP_ROM_QSTR(MP_QSTR_vsync_idle_low), MP_ROM_FALSE },
        { MP_ROM_QSTR(MP_QSTR_de_idle_high), MP_ROM_FALSE },
        { MP_ROM_QSTR(MP_QSTR_pclk_active_high), MP_ROM_FALSE },
        { MP_ROM_QSTR(MP_QSTR_pclk_idle_high), MP_ROM_FALSE },
};
MP_DEFINE_CONST_DICT(timings1024_dict, timings1024_table);

static const mp_rom_map_elem_t board_module_globals_table[] = {
        CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

        { MP_ROM_QSTR(MP_QSTR_TFT_PINS), MP_ROM_PTR(&tft_pins_dict) },
        { MP_ROM_QSTR(MP_QSTR_TFT_TIMINGS), MP_ROM_PTR(&timings1024_dict) },
        { MP_ROM_QSTR(MP_QSTR_TFT_TIMINGS1024), MP_ROM_PTR(&timings1024_dict) },
        { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_GPIO47) },

        // GPIO pins available on Mabee connector port (also shared with I2S & USB D+/D-)
        { MP_ROM_QSTR(MP_QSTR_GPIO20), MP_ROM_PTR(&pin_GPIO20) },
        { MP_ROM_QSTR(MP_QSTR_GPIO19), MP_ROM_PTR(&pin_GPIO19) },

        // I2S pins are shared with USB D+/D-, these are only useful if USB is disabled
        { MP_ROM_QSTR(MP_QSTR_I2S_BIT_CLOCK), MP_ROM_PTR(&pin_GPIO20) },
        { MP_ROM_QSTR(MP_QSTR_I2S_WORD_SELECT), MP_ROM_PTR(&pin_GPIO2) },
        { MP_ROM_QSTR(MP_QSTR_I2S_DATA), MP_ROM_PTR(&pin_GPIO19) },

        { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
        { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

        { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO18) },
        { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO17) },
        { MP_ROM_QSTR(MP_QSTR_TOUCH_RESET), MP_ROM_PTR(&pin_GPIO48) },

        // USB = D- 19, D+ 20

        { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },

        { MP_ROM_QSTR(MP_QSTR_MIDI_IN),  MP_ROM_PTR(&pin_GPIO11) },
        { MP_ROM_QSTR(MP_QSTR_MIDI_OUT), MP_ROM_PTR(&pin_GPIO12) },

        // boot mode button can be used in SW as well
        { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO1) },

        { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
