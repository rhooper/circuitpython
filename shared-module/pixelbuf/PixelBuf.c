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
#include "py/runtime.h"
#include "PixelBuf.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
} pixelbuf_rgbw_t;

static pixelbuf_rgbw_t pixelbuf_byteorder_lookup[] = {
    {0, 1, 2, 3},  // BYTEORDER_RGB
    {0, 2, 1, 3},  // BYTEORDER_RBG
    {1, 0, 2, 3},  // BYTEORDER_GRB
    {1, 2, 0, 3},  // BYTEORDER_GBR
    {2, 0, 1, 3},  // BYTEORDER_BRG
    {2, 1, 0, 3},  // BYTEORDER_BGR
};

void pixelbuf_set_pixel_int(uint8_t *buf, mp_int_t value, uint byteorder, uint bpp) {
        buf[pixelbuf_byteorder_lookup[byteorder].r] = value >> 16 & 0xff;
        buf[pixelbuf_byteorder_lookup[byteorder].g] = (value >> 8) & 0xff;
        buf[pixelbuf_byteorder_lookup[byteorder].b] = value & 0xff;
        if (bpp == 4 && (buf[0] == buf[1]) && (buf[1] == buf[2]) && (buf[2] == buf[3])) { // Assumes W is always 4th byte.
            buf[3] = buf[0];
            buf[1] = buf[2] = 0;
        }
}

void pixelbuf_set_pixel(uint8_t *buf, mp_obj_t *item, uint byteorder, uint bpp, bool dotstar) {
    if (MP_OBJ_IS_INT(item)) {
        pixelbuf_set_pixel_int(buf, mp_obj_get_int_truncated(item), byteorder, bpp);
    } else {
        mp_obj_t *items;
        size_t len;
        mp_obj_get_array(item, &len, &items);
        if (len != bpp && !dotstar) {
            mp_raise_ValueError_varg("Expected tuple of length %d, got %d", bpp, len);
        }
        buf[pixelbuf_byteorder_lookup[byteorder].r] = mp_obj_get_int_truncated(items[PIXEL_R]);
        buf[pixelbuf_byteorder_lookup[byteorder].g] = mp_obj_get_int_truncated(items[PIXEL_G]);
        buf[pixelbuf_byteorder_lookup[byteorder].b] = mp_obj_get_int_truncated(items[PIXEL_B]);
        if (len > 3) {
            if (dotstar) {
                *(buf-1) = DOTSTAR_BRIGHTNESS(mp_obj_get_float(items[PIXEL_W]));
            } else {
                buf[pixelbuf_byteorder_lookup[byteorder].w] = mp_obj_get_int_truncated(items[PIXEL_W]);
            }
        }
    }
}

mp_obj_t *pixelbuf_get_pixel_array(uint8_t *buf, uint len, uint byteorder, uint bpp) {
    mp_obj_t elems[len];
    for (uint i = 0; i < len; i++) {
        elems[i] = pixelbuf_get_pixel(buf + (i * bpp), byteorder, bpp);
    }
    return mp_obj_new_tuple(len, elems);
}

mp_obj_t *pixelbuf_get_pixel(uint8_t *buf, uint byteorder, uint bpp) {
    mp_obj_t elems[bpp];
   
    elems[0] = mp_obj_new_int(buf[pixelbuf_byteorder_lookup[byteorder].r]);
    elems[1] = mp_obj_new_int(buf[pixelbuf_byteorder_lookup[byteorder].g]);
    elems[2] = mp_obj_new_int(buf[pixelbuf_byteorder_lookup[byteorder].b]);
    if (bpp > 3)
        elems[3] = mp_obj_new_int(buf[pixelbuf_byteorder_lookup[byteorder].w]);

    return mp_obj_new_tuple(bpp, elems);
}

