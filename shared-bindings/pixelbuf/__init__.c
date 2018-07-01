/*
 * This file is part of the Circuit Python project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Roy Hooper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "types.h"
#include "__init__.h"

#include "PixelBuf.h"
#include "../../shared-module/pixelbuf/PixelBuf.h"

//| :mod:`pixelbuf` --- Pixel Buffer
//| ===========================================================
//|
//| .. module:: pixelbuf
//|   :synopsis: Pixel buffer for devices like NeoPixel and DotStar
//|
//| The `pixelbuf` module helps the Adafruit Dotstar and Adafruit Neopixel
//| libraries rapidly manipulate pixel buffers in any RGB[W] order.
//|

//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     PixelBuf

const mp_obj_type_t pixelbuf_byteorder_type = {
    { &mp_type_type },
    .name = MP_QSTR_Byteorder,
    .print = pixelbuf_byteorder_print
};

// This macro is used to simplify RGB subclass definition
#define PIXELBUF_BYTEORDER(p_name, p_bpp, p_r, p_g, p_b, p_w, p_has_white) \
const pixelbuf_rgbw_obj_t byteorder_## p_name = { \
    { &pixelbuf_byteorder_type }, \
    .name = MP_QSTR_## p_name, \
    .bpp = p_bpp, \
    .byteorder = { p_r, p_g, p_b, p_w }, \
    .has_white = p_has_white \
};

/// RGB
PIXELBUF_BYTEORDER(RGB, 3, 0, 1, 2, 3, false)
PIXELBUF_BYTEORDER(RBG, 3, 0, 2, 1, 3, false)
PIXELBUF_BYTEORDER(GRB, 3, 1, 0, 2, 3, false)
PIXELBUF_BYTEORDER(GBR, 3, 1, 2, 0, 3, false)
PIXELBUF_BYTEORDER(BRG, 3, 2, 0, 1, 3, false)
PIXELBUF_BYTEORDER(BGR, 3, 2, 1, 0, 3, false)
// RGBW
PIXELBUF_BYTEORDER(RGBW, 4, 0, 1, 2, 3, true)
PIXELBUF_BYTEORDER(RBGW, 4, 0, 2, 1, 3, true)
PIXELBUF_BYTEORDER(GRBW, 4, 1, 0, 2, 3, true)
PIXELBUF_BYTEORDER(GBRW, 4, 1, 2, 0, 3, true)
PIXELBUF_BYTEORDER(BRGW, 4, 2, 0, 1, 3, true)
PIXELBUF_BYTEORDER(BGRW, 4, 2, 1, 0, 3, true)
// Alpha (Brightness) + RGB (eg Dotstar)
PIXELBUF_BYTEORDER(ARGB, 4, 1, 2, 3, 0, false)
PIXELBUF_BYTEORDER(ARBG, 4, 1, 3, 2, 0, false)
PIXELBUF_BYTEORDER(AGRB, 4, 2, 1, 3, 0, false)
PIXELBUF_BYTEORDER(AGBR, 4, 2, 3, 1, 0, false)
PIXELBUF_BYTEORDER(ABRG, 4, 3, 1, 2, 0, false)
PIXELBUF_BYTEORDER(ABGR, 4, 3, 2, 1, 0, false)

STATIC const mp_rom_map_elem_t pixelbuf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pixelbuf) },
    { MP_ROM_QSTR(MP_QSTR_PixelBuf), MP_ROM_PTR(&pixelbuf_pixelbuf_type) },
    { MP_ROM_QSTR(MP_QSTR_ByteOrder), MP_ROM_PTR(&pixelbuf_byteorder_type) },
    { MP_ROM_QSTR(MP_QSTR_RGB),  MP_ROM_PTR(&byteorder_RGB) },
    { MP_ROM_QSTR(MP_QSTR_RBG),  MP_ROM_PTR(&byteorder_RBG) },
    { MP_ROM_QSTR(MP_QSTR_GRB),  MP_ROM_PTR(&byteorder_GRB) },
    { MP_ROM_QSTR(MP_QSTR_GBR),  MP_ROM_PTR(&byteorder_GBR) },
    { MP_ROM_QSTR(MP_QSTR_BRG),  MP_ROM_PTR(&byteorder_BRG) },
    { MP_ROM_QSTR(MP_QSTR_BGR),  MP_ROM_PTR(&byteorder_BGR) },
    { MP_ROM_QSTR(MP_QSTR_RGBW), MP_ROM_PTR(&byteorder_RGBW) },
    { MP_ROM_QSTR(MP_QSTR_RBGW), MP_ROM_PTR(&byteorder_RBGW) },
    { MP_ROM_QSTR(MP_QSTR_GRBW), MP_ROM_PTR(&byteorder_GRBW) },
    { MP_ROM_QSTR(MP_QSTR_GBRW), MP_ROM_PTR(&byteorder_GBRW) },
    { MP_ROM_QSTR(MP_QSTR_BRGW), MP_ROM_PTR(&byteorder_BRGW) },
    { MP_ROM_QSTR(MP_QSTR_BGRW), MP_ROM_PTR(&byteorder_BGRW) },
    { MP_ROM_QSTR(MP_QSTR_ARGB), MP_ROM_PTR(&byteorder_ARGB) },
    { MP_ROM_QSTR(MP_QSTR_ARBG), MP_ROM_PTR(&byteorder_ARBG) },
    { MP_ROM_QSTR(MP_QSTR_AGRB), MP_ROM_PTR(&byteorder_AGRB) },
    { MP_ROM_QSTR(MP_QSTR_AGBR), MP_ROM_PTR(&byteorder_AGBR) },
    { MP_ROM_QSTR(MP_QSTR_ABRG), MP_ROM_PTR(&byteorder_ABRG) },
    { MP_ROM_QSTR(MP_QSTR_ABGR), MP_ROM_PTR(&byteorder_ABGR) },
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_module_globals, pixelbuf_module_globals_table);

STATIC void pixelbuf_byteorder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pixelbuf_rgbw_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "%q.%q", MP_QSTR_pixelbuf, self->name);
    return;
}

const mp_obj_module_t pixelbuf_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&pixelbuf_module_globals,
};
