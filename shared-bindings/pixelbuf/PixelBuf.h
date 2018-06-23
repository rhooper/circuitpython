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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_PIXELBUF_PIXELBUF_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_PIXELBUF_PIXELBUF_H

const mp_obj_type_t pixelbuf_pixelbuf_type;

typedef struct {
    mp_obj_base_t base;
    size_t pixels;
    size_t bytes;
    size_t bpp;
    size_t pixel_step;
    size_t byteorder;
    mp_obj_t bytearray;
    mp_obj_t rawbytearray;
    mp_float_t brightness;
    bool two_buffers;
    size_t offset;
    uint8_t dotstar_mode;
    uint8_t *rawbuf;
    uint8_t *buf;
} pixelbuf_pixelbuf_obj_t;

void pixelbuf_recalculate_brightness(pixelbuf_pixelbuf_obj_t *self);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_PIXELBUF_PIXELBUF_H
