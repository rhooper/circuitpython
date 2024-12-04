
#include "py/objtuple.h"
#include "shared-bindings/board/__init__.h"
#include "shared-module/displayio/__init__.h"


/***
  * https://github.com/atomic14/esp32-s3-pinouts
   PIN NAME FUNCTION
    01 GND
    02 3V3
    03 EN   RESET   [RTS]
    04 IO4  D_G7
    05 IO5  DAC_DIN
    06 IO6  D_G3
    07 IO7  D_G4
    08 IO15 D_G5
    09 IO16 D_G6
    10 IO17 I2C_SDA MABEE_I2C_SDA
    11 IO18 I2C_SCL MABEE_I2C_SCL
    12 IO8  DAC_BCLK
    13 IO19 USB_D-  (Host)
    14 IO20 USB_D+  (Host)

    15 IO3  BAT
    16 IO46 D_B5
    17 IO9  D_B6
    18 IO10 D_R5
    19 IO11 MIDI_OUT
    20 IO12 MIDI_IN
    21 IO13 D_R4
    22 IO14 D_R7
    23 IO21 D_R6
    24 IO47 Backlight PWM
    25 IO48 TP_RST (also IO34?) // 1.8V     TP_RST
    26 IO45 D_R3

    27 IO0  RESET [DTR]
    28 ?
    29 ?
    30 ?
    31 IO38 D_B4
    32 IO39 D_HSYNC
    33 IO40 D_DE
    34 IO41 D_VSYNC
    35 IO42 D_DCLK
    36 IO44 // RXD0 to CH340K USB
    37 IO43 // TXD0 to CH340K USB
    38 IO2  DAC_LRC
    39 IO1  D_B7
    40 GND
    41 GND [EPAD]

    CH340K
     1 D+  ->
     2 D-  ->
     3 GND
     4 DTR -> RESET
     5 CTS
     6 RTS -> IO0
     7 VCC VBUS
     8 TXD -> RXD IO44
     9 RXD -> TXD IO43
    10 V3  +3V3

    USB-KEY
    USB_D+ IO19
    USB_D- IO20

    TOUCH (GT911)
    TP_INT -> pullup? pulldown?
    TP_RST -> IO48
    IO18/I2C_SCL
    IO19/I2C_SDA

    U11 - DAC?
     1 CPVDD +3V33
     2 CAPP -> CAPM
     3 CPGND -> AGND
     4 CAPM -> CAPP
     5 VNEG -> ~AGND
     6 OUTL -> LOUT [jack]
     7 OUTR -> ROUT [jack]
     8 AVDD -> +3V3
     9 AGND -> AGND [jack]
    10 DEMP -> GND
    11 FLT  GND
    12 SCK  GND
    13 BCK  BCLK
    14 DIN  DIN
    15 LRCK LRC
    16 FMT  GND
    17 XSMT -> AVDD(8) -> ROUT
    18 LDOO -> GND (via caps)
    19 DGND -> GND
    20 DVDD -> +3V3

    DISPLAY
     1 NC
     2 NC
     3 GND
     4 +3V3
     5 VCOM
     6 NC
     7 3V3
     8 AVDD10
     9 VGL
    10 VGH
    11 GND
    12 +3V3
    13 GND
    14 DCLK IO42
    15 GND
    16 R0       GND
    17 R1       GND
    18 R2       GND
    19 R3   IO45
    20 R4   IO13
    21 R5   IO10
    22 R6   IO21
    23 R7   IO14
    24 G0       GND
    25 G1       GND
    26 G2       GND
    27 G3   IO6
    28 G4   IO7
    29 G5   IO15
    30 G6   IO16
    31 G7   IO4
    32 B0       GND
    33 B1       GND
    34 B2       GND
    35 B3       GND
    36 B4   IO38
    37 B5   IO46
    38 B6   IO9
    39 B7   IO1
    40 HSYNC    IO39
    41 VSYNC    IO41
    42 DE       IO40
    43 +3V3
    44 +3V3 -||- GND
    45 VCOM
    46 VCOM
    47 LED-
    48 LED-
    49 LED+
    50 LED+
    51 GND
    52 GND

*/
// Audio: DIN IO5; BCLK IO8; LRC IO2

static const mp_rom_obj_tuple_t tft_b_pins = {
        {&mp_type_tuple},
        4,
        {
                MP_ROM_PTR(&pin_GPIO38), // B4
                MP_ROM_PTR(&pin_GPIO46), // B5
                MP_ROM_PTR(&pin_GPIO9),  // B6
                MP_ROM_PTR(&pin_GPIO1),  // B7
        }
};

static const mp_rom_obj_tuple_t tft_g_pins = {
        {&mp_type_tuple},
        5,
        {
                MP_ROM_PTR(&pin_GPIO6),  // G3
                MP_ROM_PTR(&pin_GPIO7),  // G4
                MP_ROM_PTR(&pin_GPIO15), // G5
                MP_ROM_PTR(&pin_GPIO16), // G6
                MP_ROM_PTR(&pin_GPIO4),  // G7
        }
};

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

#define AS_MP_BOOL(val) ((val) ? mp_const_true : mp_const_false)
static const mp_rom_map_elem_t timings1024_table[] = {
        { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_INT(TULIP_DISPLAY_FREQUENCY) }, // nominal 16MHz, but display is unstable/tears at that frequency
        { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_INT(TULIP_DISPLAY_WIDTH) },
        { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_INT(TULIP_DISPLAY_FREQUENCY) },
        { MP_ROM_QSTR(MP_QSTR_hsync_pulse_width), MP_ROM_INT(TULIP_DISPLAY_HSYNC_PULSE_WIDTH) },
        { MP_ROM_QSTR(MP_QSTR_hsync_front_porch), MP_ROM_INT(TULIP_DISPLAY_HSYNC_FRONT_PORCH) },
        { MP_ROM_QSTR(MP_QSTR_hsync_back_porch), MP_ROM_INT(TULIP_DISPLAY_HSYNC_BACK_PORCH) },
        { MP_ROM_QSTR(MP_QSTR_hsync_idle_low), AS_MP_BOOL(TULIP_DISPLAY_HSYNC_IDLE_LOW) },
        { MP_ROM_QSTR(MP_QSTR_vsync_pulse_width), MP_ROM_INT(TULIP_DISPLAY_VSYNC_PULSE_WIDTH) },
        { MP_ROM_QSTR(MP_QSTR_vsync_front_porch), MP_ROM_INT(TULIP_DISPLAY_VSYNC_FRONT_PORCH) },
        { MP_ROM_QSTR(MP_QSTR_vsync_back_porch), MP_ROM_INT(TULIP_DISPLAY_VSYNC_BACK_PORCH) },
        { MP_ROM_QSTR(MP_QSTR_vsync_idle_low), AS_MP_BOOL(TULIP_DISPLAY_VSYNC_IDLE_LOW) },
        { MP_ROM_QSTR(MP_QSTR_de_idle_high), AS_MP_BOOL(TULIP_DISPLAY_DE_IDLE_HIGH) },
        { MP_ROM_QSTR(MP_QSTR_pclk_active_high), AS_MP_BOOL(TULIP_DISPLAY_PCLK_ACTIVE_HIGH) },
        { MP_ROM_QSTR(MP_QSTR_pclk_idle_high), AS_MP_BOOL(TULIP_DISPLAY_PCLK_IDLE_HIGH) },
};
MP_DEFINE_CONST_DICT(timings1024_dict, timings1024_table);

static const mp_rom_map_elem_t board_module_globals_table[] = {
        CIRCUITPYTHON_BOARD_DICT_STANDARD_ITEMS

        { MP_ROM_QSTR(MP_QSTR_TFT_PINS), MP_ROM_PTR(&tft_pins_dict) },
        { MP_ROM_QSTR(MP_QSTR_TFT_TIMINGS), MP_ROM_PTR(&timings1024_dict) },
        { MP_ROM_QSTR(MP_QSTR_TFT_BACKLIGHT), MP_ROM_PTR(&pin_GPIO47) },

        { MP_ROM_QSTR(MP_QSTR_TOUCH_RESET), MP_ROM_PTR(&pin_GPIO48) },

        { MP_ROM_QSTR(MP_QSTR_I2S_BIT_CLOCK), MP_ROM_PTR(&pin_GPIO8) },
        { MP_ROM_QSTR(MP_QSTR_I2S_WORD_SELECT), MP_ROM_PTR(&pin_GPIO2) },
        { MP_ROM_QSTR(MP_QSTR_I2S_DATA), MP_ROM_PTR(&pin_GPIO5) },

        // { MP_ROM_QSTR(MP_QSTR_TX), MP_ROM_PTR(&pin_GPIO43) },
        // { MP_ROM_QSTR(MP_QSTR_RX), MP_ROM_PTR(&pin_GPIO44) },

        // { MP_ROM_QSTR(MP_QSTR_SCL), MP_ROM_PTR(&pin_GPIO18) },
        // { MP_ROM_QSTR(MP_QSTR_SDA), MP_ROM_PTR(&pin_GPIO17) },

        // USB = D- 19, D+ 20

        { MP_ROM_QSTR(MP_QSTR_DISPLAY), MP_ROM_PTR(&displays[0].display) },

        { MP_ROM_QSTR(MP_QSTR_MIDI_IN),  MP_ROM_PTR(&pin_GPIO11) },
        { MP_ROM_QSTR(MP_QSTR_MIDI_OUT), MP_ROM_PTR(&pin_GPIO12) },

        // boot mode button can be used in SW as well
        // { MP_ROM_QSTR(MP_QSTR_BOOT0), MP_ROM_PTR(&pin_GPIO0) },
        // { MP_ROM_QSTR(MP_QSTR_BUTTON), MP_ROM_PTR(&pin_GPIO1) },

        { MP_ROM_QSTR(MP_QSTR_I2C), MP_ROM_PTR(&board_i2c_obj) },
        { MP_ROM_QSTR(MP_QSTR_UART), MP_ROM_PTR(&board_uart_obj) },
};
MP_DEFINE_CONST_DICT(board_module_globals, board_module_globals_table);
