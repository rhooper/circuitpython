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

void pixelbuf_set_pixel_int(uint8_t *buf, mp_int_t value, pixelbuf_rgbw_obj_t *byteorder) {
        buf[byteorder->byteorder.r] = value >> 16 & 0xff;
        buf[byteorder->byteorder.g] = (value >> 8) & 0xff;
        buf[byteorder->byteorder.b] = value & 0xff;
        if (byteorder->bpp == 4 && byteorder->has_white && 
                (buf[byteorder->byteorder.r] == buf[byteorder->byteorder.g] && 
                 buf[byteorder->byteorder.r] == buf[byteorder->byteorder.b])) {
            buf[byteorder->byteorder.w] = buf[byteorder->byteorder.r];
            buf[byteorder->byteorder.r] = buf[byteorder->byteorder.g] = buf[byteorder->byteorder.b] = 0;
        }
}

void pixelbuf_set_pixel(uint8_t *buf, mp_obj_t *item, pixelbuf_rgbw_obj_t *byteorder, bool dotstar) {
    if (MP_OBJ_IS_INT(item)) {
        pixelbuf_set_pixel_int(buf, mp_obj_get_int_truncated(item), byteorder);
    } else {
        mp_obj_t *items;
        size_t len;
        mp_obj_get_array(item, &len, &items);
        if (len != byteorder->bpp && !dotstar) {
            mp_raise_ValueError_varg("Expected tuple of length %d, got %d", byteorder->bpp, len);
        }
        buf[byteorder->byteorder.r] = mp_obj_get_int_truncated(items[PIXEL_R]);
        buf[byteorder->byteorder.g] = mp_obj_get_int_truncated(items[PIXEL_G]);
        buf[byteorder->byteorder.b] = mp_obj_get_int_truncated(items[PIXEL_B]);
        if (len > 3) {
            if (dotstar) {
                *(buf-1) = DOTSTAR_LED_START | DOTSTAR_BRIGHTNESS(mp_obj_get_float(items[PIXEL_W]));
            } else {
                buf[byteorder->byteorder.w] = mp_obj_get_int_truncated(items[PIXEL_W]);
            }
        } else if (dotstar) {
            *(buf-1) = DOTSTAR_LED_START_FULL_BRIGHT;
        }
    }
}

mp_obj_t *pixelbuf_get_pixel_array(uint8_t *buf, uint len, pixelbuf_rgbw_obj_t *byteorder, uint8_t step) {
    mp_obj_t elems[len];
    for (uint i = 0; i < len; i++) {
        elems[i] = pixelbuf_get_pixel(buf + (i * step), byteorder);
    }
    return mp_obj_new_tuple(len, elems);
}

mp_obj_t *pixelbuf_get_pixel(uint8_t *buf, pixelbuf_rgbw_obj_t *byteorder) {
    mp_obj_t elems[byteorder->bpp];
   
    elems[0] = mp_obj_new_int(buf[byteorder->byteorder.r]);
    elems[1] = mp_obj_new_int(buf[byteorder->byteorder.g]);
    elems[2] = mp_obj_new_int(buf[byteorder->byteorder.b]);
    if (byteorder->bpp > 3)
        elems[3] = mp_obj_new_int(buf[byteorder->byteorder.w]);

    return mp_obj_new_tuple(byteorder->bpp, elems);
}
