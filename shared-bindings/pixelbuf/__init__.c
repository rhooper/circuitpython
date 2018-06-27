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

#include "shared-bindings/pixelbuf/PixelBuf.h"
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


//| .. method:: wheel(pos)
//|
//|   Return an RGB color tuple at position 0-255 from a color wheel.
//|   Gives a nice color swirl.
//|   Input a value 0 to 255 to get a color value.
//|   The colours are a transition r - g - b - back to r.
//|
//|   :param float pos: position in wheel
//|
STATIC mp_obj_t pixelbuf_color_wheel(mp_obj_t pos_o) {
    float pos = mp_obj_get_float(pos_o);
    return color_wheel(pos);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_color_wheel_obj, pixelbuf_color_wheel);


STATIC const mp_rom_map_elem_t pixelbuf_module_globals_table[] = {
{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pixelbuf) },
{ MP_ROM_QSTR(MP_QSTR_PixelBuf), MP_ROM_PTR(&pixelbuf_pixelbuf_type) },
{ MP_ROM_QSTR(MP_QSTR_wheel), MP_ROM_PTR(&pixelbuf_color_wheel_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_module_globals, pixelbuf_module_globals_table);


const mp_obj_module_t pixelbuf_module = {
        .base = { &mp_type_module },
        .globals = (mp_obj_dict_t*)&pixelbuf_module_globals,
};
