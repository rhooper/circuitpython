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
#include "py/objarray.h"

#ifndef PIXELBUF_SHARED_MODULE_H
#define PIXELBUF_SHARED_MODULE_H


#define BYTEORDER_RGB 0
#define BYTEORDER_RBG 1
#define BYTEORDER_GRB 2
#define BYTEORDER_GBR 3
#define BYTEORDER_BRG 4
#define BYTEORDER_BGR 5

#define PIXEL_R 0
#define PIXEL_G 1
#define PIXEL_B 2
#define PIXEL_W 3

void pixelbuf_set_pixel(uint8_t *buf, mp_obj_t *item, uint byteorder, uint bpp);
mp_obj_t *pixelbuf_get_pixel(uint8_t *buf, uint byteorder, uint bpp);
mp_obj_t *pixelbuf_get_pixel_array(uint8_t *buf, uint len, uint byteorder, uint bpp);
void pixelbuf_set_pixel_int(uint8_t *buf, mp_int_t value, uint byteorder, uint bpp);

#endif
