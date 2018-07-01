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
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/objproperty.h"

#include <string.h>

#include "PixelBuf.h"
#include "shared-bindings/pixelbuf/types.h"
#include "../../shared-module/pixelbuf/PixelBuf.h"

extern const pixelbuf_rgbw_obj_t byteorder_BGR;
extern const mp_obj_type_t pixelbuf_byteorder_type;

//| .. currentmodule:: pixelbuf
//|
//| :class:`PixelBuf` -- An RGB[W] Pixel Buffer
//| ===========================================
//|
//| :class:`~pixelbuf.PixelBuf` implements an RGB[W] bytearray abstraction.
//|
//| .. class:: PixelBuf(size, buf, byteorder=BGR, bpp=3)
//|
//|   Create a PixelBuf object of the specified size, byteorder, and bits per pixel.
//|
//|   When given a second bytearray (`rawbuf`), changing brightness adjusts the
//|   brightness of all members of `buf`.
//|
//|   When only given `buf`, `brightness` applies to the next pixel assignment.
//|
//|   When `dotstar` is true, and `bpp` is 4, the 4th value in a tuple/list
//|   is the individual pixel brightness (0-1).  `brightness` is also applied if < 
//|   brightness 1.0.  Dotstar mode also puts the APA102 start frame before RGB
//|   values.
//|
//|   :param ~int size: Number of pixels
//|   :param ~bytearray buf: Bytearray to store pixel data in
//|   :param ~pixelbuf.ByteOrder byteorder: Byte order constant from `pixelbuf`
//|   :param ~int bpp: Bytes per pixel
//|   :param ~float brightness: Brightness (0 to 1)
//|   :param ~bytearray rawbuf: Bytearray to store raw pixel colors in
//|   :param ~offset int: Offset from start of buffer (default 0)
//|   :param ~dotstar bool: Dotstar mode.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 2, MP_OBJ_FUN_ARGS_MAX, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_size, ARG_buf, ARG_byteorder, ARG_bpp, ARG_brightness, ARG_rawbuf, ARG_offset, ARG_dotstar };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buf, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_byteorder, MP_ARG_INT, { .u_obj = mp_const_none } },
        { MP_QSTR_bpp, MP_ARG_INT, { .u_int = 3 } },
        { MP_QSTR_brightness, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_rawbuf, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_offset, MP_ARG_INT, { .u_int = 0 } },
        { MP_QSTR_dotstar, MP_ARG_BOOL, { .u_bool = false } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t bpp = args[ARG_bpp].u_int;
    size_t effective_bpp = args[ARG_dotstar].u_bool ? 4 : bpp; // Always 4 for DotStar
    size_t size = args[ARG_size].u_int * effective_bpp;

    mp_buffer_info_t bufinfo, rawbufinfo;

    if (args[ARG_byteorder].u_obj != mp_const_none && 
        !MP_OBJ_IS_TYPE(args[ARG_byteorder].u_obj, &pixelbuf_byteorder_type)) 
    {
        mp_raise_TypeError("byteorder is not an instance of ByteOrder");
    }

    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_READ | MP_BUFFER_WRITE);
    bool two_buffers = args[ARG_rawbuf].u_obj != mp_const_none;
    if (two_buffers) {
        mp_get_buffer_raise(args[ARG_rawbuf].u_obj, &rawbufinfo, MP_BUFFER_READ | MP_BUFFER_WRITE);
        if (rawbufinfo.len != bufinfo.len) {
            mp_raise_ValueError("rawbuf is not the same size as buf");
        }
    }

    if (size + args[ARG_offset].u_int > bufinfo.len) {
        mp_raise_ValueError_varg("buf is too small.  Need at least %d bytes.", size);
    }

    pixelbuf_pixelbuf_obj_t *self = m_new_obj(pixelbuf_pixelbuf_obj_t);

    self->base.type = &pixelbuf_pixelbuf_type;
    self->pixels = args[ARG_size].u_int;
    self->bpp = bpp;
    self->bytes = size;
    if (args[ARG_byteorder].u_obj == mp_const_none) {
        self->byteorder = MP_OBJ_FROM_PTR(&byteorder_BGR);
    } else {
        self->byteorder = args[ARG_byteorder].u_obj;
    }
    self->bytearray = args[ARG_buf].u_obj;
    self->two_buffers = two_buffers;
    self->offset = args[ARG_offset].u_int;
    self->dotstar_mode = args[ARG_dotstar].u_bool;
    self->buf = bufinfo.buf;
    self->buf += self->offset;
    self->rawbuf = two_buffers ? rawbufinfo.buf : NULL;
    if (self->rawbuf)
        self->rawbuf += self->offset;
    self->pixel_step = effective_bpp;  

    if (args[ARG_brightness].u_obj == mp_const_none) {
        self->brightness = 1.0;
    } else {
        self->brightness = mp_obj_get_float(args[ARG_brightness].u_obj);
        if (self->brightness < 0)
            self->brightness = 0;
        else if (self->brightness > 1)
            self->brightness = 1;
    }
    
    if (self->dotstar_mode) { 
        // Offset the buffer by 1 in dotstar mode, as the dotstar needs a start byte with the start
        // bits and brightness bits.
        for (uint i = 0; i < self->pixels * 4; i += 4) {
            self->buf[i] = DOTSTAR_LED_START_FULL_BRIGHT;
            if (two_buffers) {
                self->rawbuf[i] = DOTSTAR_LED_START_FULL_BRIGHT;
            }
        }
        self->buf += 1;
        self->rawbuf += 1;
        self->bpp = 3; // Always 3 bpp (RGB) in DotStar mode
    }

    if (two_buffers) {
        self->rawbytearray = args[ARG_rawbuf].u_obj;
    }
    
    return MP_OBJ_FROM_PTR(self);
}

//|   .. attribute:: bpp
//|
//|     The number of bytes per pixel in the buffer (read-only)
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_bpp(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int_from_uint(self->bpp);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_bpp_obj, pixelbuf_pixelbuf_obj_get_bpp);

const mp_obj_property_t pixelbuf_pixelbuf_bpp_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_bpp_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};


//|   .. attribute:: brightness
//|
//|     Float value between 0 and 1.  Output brightness.
//|     If the PixelBuf was allocated with two both a buf and a rawbuf,
//|     setting this value causes a recomputation of the values in buf.
//|     If only a buf was provided, then the brightness only applies to
//|     future pixel changes.
//|     In DotStar mode 
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_brightness(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(self->brightness);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_brightness_obj, pixelbuf_pixelbuf_obj_get_brightness);


STATIC mp_obj_t pixelbuf_pixelbuf_obj_set_brightness(mp_obj_t self_in, mp_obj_t value) {
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->brightness = mp_obj_float_get(value);
    if (self->brightness > 1)
        self->brightness = 1;
    else if (self->brightness < 0)
        self->brightness = 0;
    if (self->two_buffers)
        pixelbuf_recalculate_brightness(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pixelbuf_pixelbuf_set_brightness_obj, pixelbuf_pixelbuf_obj_set_brightness);

const mp_obj_property_t pixelbuf_pixelbuf_brightness_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_brightness_obj,
              (mp_obj_t)&pixelbuf_pixelbuf_set_brightness_obj,
              (mp_obj_t)&mp_const_none_obj},
};

void pixelbuf_recalculate_brightness(pixelbuf_pixelbuf_obj_t *self) {
    uint8_t *buf = (uint8_t *)self->buf;
    uint8_t *rawbuf = (uint8_t *)self->rawbuf;
    if (self->dotstar_mode) {
        buf--;
        rawbuf--;
    }
    for (uint i = 0; i < self->bytes; i++) {
        // Don't adjust start/brightness bytes in dotstar mode
        if (self->dotstar_mode && (i % 4 == 0)) {
            buf[i] = rawbuf[i];
        } else {
            buf[i] = rawbuf[i] * self->brightness;
        }
    }
}

//|   .. attribute:: buf
//|
//|     The bytearray of pixel data after brightness adjustment
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_buf(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bytearray_by_ref(self->bytes, self->buf);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_buf_obj, pixelbuf_pixelbuf_obj_get_buf);

const mp_obj_property_t pixelbuf_pixelbuf_buf_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_buf_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: byteorder
//|
//|     byteorder for the buffer (read-only)
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_byteorder(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->byteorder;
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_byteorder_obj, pixelbuf_pixelbuf_obj_get_byteorder);

const mp_obj_property_t pixelbuf_pixelbuf_byteorder_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_byteorder_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC mp_obj_t pixelbuf_pixelbuf_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (op) {
        case MP_UNARY_OP_BOOL: return mp_obj_new_bool(self->byteorder != 0);
        case MP_UNARY_OP_LEN: return MP_OBJ_NEW_SMALL_INT(self->pixels);
        default: return MP_OBJ_NULL; // op not supported
    }
}


STATIC const mp_rom_map_elem_t pixelbuf_pixelbuf_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_bpp), MP_ROM_PTR(&pixelbuf_pixelbuf_bpp_obj)},
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&pixelbuf_pixelbuf_brightness_obj)},
    { MP_ROM_QSTR(MP_QSTR_buf), MP_ROM_PTR(&pixelbuf_pixelbuf_buf_obj)},
    { MP_ROM_QSTR(MP_QSTR_byteorder), MP_ROM_PTR(&pixelbuf_pixelbuf_byteorder_obj)},
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_pixelbuf_locals_dict, pixelbuf_pixelbuf_locals_dict_table);


STATIC mp_obj_t pixelbuf_pixelbuf_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_NULL) {
        // delete item
        // slice deletion
        return MP_OBJ_NULL; // op not supported
    } else {
        pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
        if (0) {
#if MICROPY_PY_BUILTINS_SLICE
        } else if (MP_OBJ_IS_TYPE(index_in, &mp_type_slice)) {
            mp_bound_slice_t slice;
            mp_buffer_info_t bufinfo;
            if (!mp_seq_get_fast_slice_indexes(self->bytes, index_in, &slice)) {
                mp_raise_NotImplementedError("Only slices with step=1 (aka None) are supported");
            }
            if ((slice.stop * self->pixel_step) > self->bytes) 
                mp_raise_IndexError("Range beyond bounds of pixel buffer");

            if (value != MP_OBJ_SENTINEL) {
                #if MICROPY_PY_ARRAY_SLICE_ASSIGN
                size_t dst_len = slice.stop - slice.start;
                uint8_t *destbuf = NULL, *adjustedbuf = NULL;
                if (self->two_buffers) {
                    destbuf = self->rawbuf;
                    adjustedbuf = self->buf;
                } else {
                    destbuf = self->buf;
                }
                if (MP_OBJ_IS_TYPE(value, &mp_type_list) ||
                        MP_OBJ_IS_TYPE(value, &mp_type_tuple)) {
                    mp_obj_t *src_objs;
                    size_t num_items;
                    if (MP_OBJ_IS_TYPE(value, &mp_type_list)) {
                        mp_obj_list_t *t = MP_OBJ_TO_PTR(value);
                        num_items = t->len;
                        src_objs = t->items;
                    } else {
                        mp_obj_tuple_t *l = MP_OBJ_TO_PTR(value);
                        num_items = l->len;
                        src_objs = l->items;
                    }
                    if (num_items != dst_len) {
                        mp_raise_ValueError_varg("Unmatched number of items on RHS (expected %d, got %d).", 
                                                 dst_len, num_items);
                    }
                    for (size_t i = slice.start; i < slice.stop; i++) {
                        mp_obj_t *item = src_objs[i-slice.start];
                        if (MP_OBJ_IS_TYPE(value, &mp_type_list) ||
                                MP_OBJ_IS_TYPE(value, &mp_type_tuple) ||
                                MP_OBJ_IS_INT(value)) {
                            pixelbuf_set_pixel(destbuf + (i * self->pixel_step), item, self->byteorder, self->dotstar_mode);
                            if (self->two_buffers) {
                                if (self->dotstar_mode)
                                    *(adjustedbuf-1) = *(destbuf-1);
                                for (uint j = 0; j < self->bpp; j++) {
                                    adjustedbuf[(i * self->pixel_step) + j] = (destbuf[(i * self->pixel_step) + j] * self->brightness);
                                }
                            } else {
                                for (uint j = 0; j < self->bpp; j++) {
                                    destbuf[(i * self->pixel_step) + j] = (destbuf[(i * self->pixel_step) + j] * self->brightness);
                                }
                            }
                        }
                    }
                    return mp_const_none;
                } else {
                    mp_raise_ValueError("tuple/list required on RHS");
                }
                #else
                return MP_OBJ_NULL; // op not supported
                #endif
            } else {
                // Read slice.
                size_t len = slice.stop - slice.start;
                mp_get_buffer_raise(self->two_buffers ? self->rawbytearray : self->bytearray, &bufinfo, MP_BUFFER_READ);
                return pixelbuf_get_pixel_array((uint8_t *) bufinfo.buf + slice.start, len, self->byteorder, self->pixel_step);
            }
#endif
        } else {
            // Single index rather than slice.
            size_t index = mp_get_index(self->base.type, self->pixels, index_in, false);
            size_t offset = (index * self->pixel_step);
            if (offset > self->bytes) 
                mp_raise_IndexError("Pixel beyond bounds of buffer");
            uint8_t *pixelstart = (uint8_t *)(self->two_buffers ? self->rawbuf : self->buf) + offset;
            if (value == MP_OBJ_SENTINEL) {
                // load
                return pixelbuf_get_pixel(pixelstart, self->byteorder);
            } else {
                // store
                pixelbuf_set_pixel(pixelstart, value, self->byteorder, self->dotstar_mode);
                if (self->two_buffers) {
                    uint8_t *adjustedstart = self->buf + offset;
                    if (self->dotstar_mode)
                        *(adjustedstart-1) = *(pixelstart-1);
                    for (uint j = 0; j < self->bpp; j++) {
                        adjustedstart[j] = (pixelstart[j] * self->brightness);
                    }
                } else {
                    for (uint j = 0; j < self->bpp; j++) {
                        pixelstart[j] = (pixelstart[j] * self->brightness);
                    }
                }
                return mp_const_none;
            }
        }
    }
}

const mp_obj_type_t pixelbuf_pixelbuf_type = {
        { &mp_type_type },
        .name = MP_QSTR_PixelBuf,
        .subscr = pixelbuf_pixelbuf_subscr,
        .make_new = pixelbuf_pixelbuf_make_new,
        .unary_op = pixelbuf_pixelbuf_unary_op,
        .print = NULL,
        .locals_dict = (mp_obj_t)&pixelbuf_pixelbuf_locals_dict,
};
