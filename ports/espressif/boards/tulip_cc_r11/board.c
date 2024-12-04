// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 Rose Hooper
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "shared-bindings/board/__init__.h"
#include "shared-bindings/dotclockframebuffer/DotClockFramebuffer.h"
#include "shared-bindings/dotclockframebuffer/__init__.h"
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-module/displayio/__init__.h"
#include "bindings/espidf/__init__.h"
#include "py/runtime.h"
#include "shared-module/os/__init__.h"

static const mcu_pin_obj_t *blue_pins[] = {
    &pin_GPIO38, // B4
    &pin_GPIO46, // B5
    &pin_GPIO9,  // B6
    &pin_GPIO1   // B7
};

static const mcu_pin_obj_t *green_pins[] = {
    &pin_GPIO6,  // G3
    &pin_GPIO7,  // G4
    &pin_GPIO15, // G5
    &pin_GPIO16, // G6
    &pin_GPIO4   // G7
};

static const mcu_pin_obj_t *red_pins[] = {
    &pin_GPIO45, // R3
    &pin_GPIO13, // R4
    &pin_GPIO10, // R5
    &pin_GPIO21, // R6
    &pin_GPIO14  // R7
};

static void display_init(void) {

    mp_int_t height = 0, width = 0, frequency = 0;

    width = TULIP_DISPLAY_WIDTH;
    height = TULIP_DISPLAY_HEIGHT;
    frequency = TULIP_DISPLAY_FREQUENCY;

    dotclockframebuffer_framebuffer_obj_t *framebuffer = &allocate_display_bus_or_raise()->dotclock;
    framebuffer->base.type = &dotclockframebuffer_framebuffer_type;

    common_hal_dotclockframebuffer_framebuffer_construct(
        framebuffer,
        &pin_GPIO40,    // de
        &pin_GPIO41,    // vsync
        &pin_GPIO39,    // hsync
        &pin_GPIO42,    // pclk
        red_pins, MP_ARRAY_SIZE(red_pins),
        green_pins, MP_ARRAY_SIZE(green_pins),
        blue_pins, MP_ARRAY_SIZE(blue_pins),
        frequency,        // Frequency
        width,            // width
        height,            // height
        // horiz: pulse, back porch, front porch, idle low
        TULIP_DISPLAY_HSYNC_PULSE_WIDTH,
        TULIP_DISPLAY_HSYNC_FRONT_PORCH,
        TULIP_DISPLAY_HSYNC_BACK_PORCH,
        TULIP_DISPLAY_HSYNC_IDLE_LOW != mp_const_false,
        TULIP_DISPLAY_VSYNC_PULSE_WIDTH,
        TULIP_DISPLAY_VSYNC_FRONT_PORCH,
        TULIP_DISPLAY_VSYNC_BACK_PORCH,
        false,  // vert: pulse, back porch, front porch, idle low
        false,          // DE idle high
        false,          // pclk active high
        false,          // pclk idle high
        0               // overscan left
        );

    framebufferio_framebufferdisplay_obj_t *display = &allocate_display_or_raise()->framebuffer_display;
    display->base.type = &framebufferio_framebufferdisplay_type;
    common_hal_framebufferio_framebufferdisplay_construct(
        display,
        framebuffer,
        0,    // rotation
        true  // auto-refresh
        );
}

void board_init(void) {
    display_init();
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
