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

static pixelbuf_rgbw_t pixelbuf_byteorder_lookup[] = {
    {0, 1, 2, 3},  // BYTEORDER_RGB
    {0, 2, 1, 3},  // BYTEORDER_RBG
    {1, 0, 2, 3},  // BYTEORDER_GRB
    {1, 2, 0, 3},  // BYTEORDER_GBR
    {2, 0, 1, 3},  // BYTEORDER_BRG
    {2, 1, 0, 3},  // BYTEORDER_BGR
};

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
    for (uint i = 0; i < self->bytes; i++) {
       self->buf[i] = self->rawbuf[i] * self->brightness;
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
    return mp_obj_new_int_from_uint(self->byteorder);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_byteorder_obj, pixelbuf_pixelbuf_obj_get_byteorder);

const mp_obj_property_t pixelbuf_pixelbuf_byteorder_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_byteorder_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: rawbuf
//|
//|     The raw bytearray of pixel data if provided during construction.
//|
STATIC mp_obj_t pixelbuf_pixelbuf_obj_get_rawbuf(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->rawbuf == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_bytearray_by_ref(self->bytes, self->rawbuf);
}
MP_DEFINE_CONST_FUN_OBJ_1(pixelbuf_pixelbuf_get_rawbuf_obj, pixelbuf_pixelbuf_obj_get_rawbuf);

const mp_obj_property_t pixelbuf_pixelbuf_rawbuf_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&pixelbuf_pixelbuf_get_rawbuf_obj,
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
    { MP_ROM_QSTR(MP_QSTR_RGB), MP_ROM_INT(BYTEORDER_RGB) },
    { MP_ROM_QSTR(MP_QSTR_RBG), MP_ROM_INT(BYTEORDER_RBG) },
    { MP_ROM_QSTR(MP_QSTR_GRB), MP_ROM_INT(BYTEORDER_GRB) },
    { MP_ROM_QSTR(MP_QSTR_GBR), MP_ROM_INT(BYTEORDER_GBR) },
    { MP_ROM_QSTR(MP_QSTR_BRG), MP_ROM_INT(BYTEORDER_BRG) },
    { MP_ROM_QSTR(MP_QSTR_BGR), MP_ROM_INT(BYTEORDER_BGR) },
    { MP_ROM_QSTR(MP_QSTR_bpp), MP_ROM_PTR(&pixelbuf_pixelbuf_bpp_obj)},
    { MP_ROM_QSTR(MP_QSTR_brightness), MP_ROM_PTR(&pixelbuf_pixelbuf_brightness_obj)},
    { MP_ROM_QSTR(MP_QSTR_buf), MP_ROM_PTR(&pixelbuf_pixelbuf_buf_obj)},
    { MP_ROM_QSTR(MP_QSTR_byteorder), MP_ROM_PTR(&pixelbuf_pixelbuf_byteorder_obj)},
    { MP_ROM_QSTR(MP_QSTR_rawbuf), MP_ROM_PTR(&pixelbuf_pixelbuf_rawbuf_obj)},
};

STATIC MP_DEFINE_CONST_DICT(pixelbuf_pixelbuf_locals_dict, pixelbuf_pixelbuf_locals_dict_table);

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
//|   :param ~int size: Number of pixels
//|   :param ~bytearray buf: Bytearray to store pixel data in
//|   :param ~pixelbuf.BGR: Byte order
//|   :param ~int: Bytes per pixel
//|
STATIC mp_obj_t pixelbuf_pixelbuf_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_arg_check_num(n_args, n_kw, 2, MP_OBJ_FUN_ARGS_MAX, true);
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, pos_args + n_args);
    enum { ARG_size, ARG_buf, ARG_byteorder, ARG_bpp, ARG_brightness, ARG_rawbuf };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_size, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_buf, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_byteorder, MP_ARG_INT, { .u_int = BYTEORDER_BGR } },
        { MP_QSTR_bpp, MP_ARG_INT, { .u_int = 3 } },
        { MP_QSTR_brightness, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_rawbuf, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, &kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t bpp = args[ARG_bpp].u_int;
    size_t size = args[ARG_size].u_int * bpp;

    mp_buffer_info_t bufinfo, rawbufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_READ | MP_BUFFER_WRITE);
    bool two_buffers = args[ARG_rawbuf].u_obj != mp_const_none;
    if (two_buffers) {
        mp_get_buffer_raise(args[ARG_rawbuf].u_obj, &rawbufinfo, MP_BUFFER_READ | MP_BUFFER_WRITE);
        if (rawbufinfo.len != bufinfo.len) {
            mp_raise_ValueError("rawbuf is not the same size as buf");
        }
    }
    
    if (size > bufinfo.len) {
        mp_raise_ValueError_varg("buf is too small.  Need at least %d bytes.", size);
    }

    pixelbuf_pixelbuf_obj_t *self = m_new_obj(pixelbuf_pixelbuf_obj_t);

    self->base.type = &pixelbuf_pixelbuf_type;
    self->pixels = args[ARG_size].u_int;
    self->bpp = bpp;
    self->bytes = size;
    self->byteorder = args[ARG_byteorder].u_int;
    self->bytearray = args[ARG_buf].u_obj;
    self->buf = bufinfo.buf;
    self->rawbuf = two_buffers ? rawbufinfo.buf : NULL;
    self->two_buffers = two_buffers;

    if (args[ARG_brightness].u_obj == mp_const_none) {
        self->brightness = 1.0;
    } else {
        self->brightness = mp_obj_get_float(args[ARG_brightness].u_obj);
    }

    if (two_buffers) {
        self->rawbytearray = args[ARG_rawbuf].u_obj;
    }
    
    return MP_OBJ_FROM_PTR(self);
}



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
                mp_raise_NotImplementedError("only slices with step=1 (aka None) are supported");
            }
            if ((slice.stop * self->bpp) > self->bytes) 
                mp_raise_IndexError("Range beyond bounds of pixel buffer");

            if (value != MP_OBJ_SENTINEL) {
                #if MICROPY_PY_ARRAY_SLICE_ASSIGN
                size_t dst_len = slice.stop - slice.start;
                size_t dst_size = dst_len * self->bpp;
                uint8_t* src_items;
                uint8_t *destbuf = NULL, *adjustedbuf = NULL;
                if (self->two_buffers) {
                    destbuf = self->rawbuf;
                    adjustedbuf = self->buf;
                } else {
                    destbuf = self->buf;
                }
                if (MP_OBJ_IS_TYPE(value, &mp_type_array) ||
                        MP_OBJ_IS_TYPE(value, &mp_type_bytearray) ||
                        MP_OBJ_IS_TYPE(value, &mp_type_memoryview) ||
                        MP_OBJ_IS_TYPE(value, &mp_type_bytes)) {
                    mp_get_buffer_raise(value, &bufinfo, MP_BUFFER_READ);
                    if (1 != mp_binary_get_size('@', bufinfo.typecode, NULL)) {
                        mp_raise_ValueError("Array values should be single bytes.");
                    }
                    dst_size = dst_len;
                    if (bufinfo.len != dst_size) {
                        mp_raise_ValueError_varg("Unmatched number of bytes on RHS (expected %d, got %d).", 
                                                 dst_size, bufinfo.len);
                    }
                    src_items = bufinfo.buf;
                } else if (MP_OBJ_IS_TYPE(value, &mp_type_list) ||
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
                            pixelbuf_set_pixel(destbuf + (i * self->bpp), item, self->byteorder, self->bpp);
                            if (self->two_buffers) {
                                for (uint j = 0; j < self->bpp; j++) {
                                    adjustedbuf[(i * self->bpp) + j] = (destbuf[(i * self->bpp) + j] * self->brightness);
                                }
                            } else {
                                for (uint j = 0; j < self->bpp; j++) {
                                    destbuf[(i * self->bpp) + j] = (destbuf[(i * self->bpp) + j] * self->brightness);
                                }
                            }
                        }
                    }
                    return mp_const_none;
                } else {
                    mp_raise_NotImplementedError("array/bytes/tuple/list required on right side");
                }

                // "direct" pixel assignment
                if (slice.start % self->bpp != 0 || slice.stop % self->bpp != 0) 
                    mp_raise_IndexError("Indices must align with pixel boundaries");

                for (uint i=slice.start; i < slice.stop; i += self->bpp) {
                    mp_obj_t items[4];
                    items[0] = MP_OBJ_NEW_SMALL_INT(src_items[i]);
                    items[1] = MP_OBJ_NEW_SMALL_INT(src_items[i+1]);
                    items[2] = MP_OBJ_NEW_SMALL_INT(src_items[i+2]);
                    if (self->bpp == 4)
                        items[3] = MP_OBJ_NEW_SMALL_INT(src_items[i+3]);

                    mp_obj_t *tuple = mp_obj_new_tuple(self->bpp, items);
                    pixelbuf_set_pixel(destbuf + i, tuple, self->byteorder, self->bpp);
                    // this should probbly be optimized and refactored
                    if (self->two_buffers) {
                        for (uint j = 0; j < self->bpp; j++) {
                            adjustedbuf[i + j] = (destbuf[i + j] * self->brightness);
                        }
                    } else {
                        for (uint j = 0; j < self->bpp; j++) {
                            destbuf[i + j] = (destbuf[i + j] * self->brightness);
                        }
                    }
                }
                return mp_const_none;
                #else
                return MP_OBJ_NULL; // op not supported
                #endif
            } else {
                // Read slice.
                size_t len = slice.stop - slice.start;
                mp_get_buffer_raise(self->two_buffers ? self->rawbytearray : self->bytearray, &bufinfo, MP_BUFFER_READ);
                //return mp_obj_new_bytearray(len, (uint8_t *) bufinfo.buf + slice.start);
                return pixelbuf_get_pixel_array((uint8_t *) bufinfo.buf + slice.start, len, self->byteorder, self->bpp);
            }
#endif
        } else {
            // Single index rather than slice.
            size_t index = mp_get_index(self->base.type, self->pixels, index_in, false);
            size_t offset = (index * self->bpp);
            if (offset + self->bpp > self->bytes) 
                mp_raise_IndexError("Pixel beyond bounds of buffer");
            uint8_t *pixelstart = (uint8_t *)(self->two_buffers ? self->rawbuf : self->buf) + offset;
            if (value == MP_OBJ_SENTINEL) {
                // load
                return pixelbuf_get_pixel(pixelstart, self->byteorder, self->bpp);
            } else {
                // store
                pixelbuf_set_pixel(pixelstart, value, self->byteorder, self->bpp);
                if (self->two_buffers) {
                    uint8_t *adjustedstart = self->buf + offset;
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

void pixelbuf_set_pixel_int(uint8_t *buf, mp_int_t value, uint byteorder, uint bpp) {
        buf[pixelbuf_byteorder_lookup[byteorder].r] = value >> 16 & 0xff;
        buf[pixelbuf_byteorder_lookup[byteorder].g] = (value >> 8) & 0xff;
        buf[pixelbuf_byteorder_lookup[byteorder].b] = value & 0xff;
        if (bpp == 4 && (buf[0] == buf[1]) && (buf[1] == buf[2]) && (buf[2] == buf[3])) { // Assumes W is always 4th byte.
            buf[3] = buf[0];
            buf[1] = buf[2] = 0;
        }
}

void pixelbuf_set_pixel(uint8_t *buf, mp_obj_t *item, uint byteorder, uint bpp) {
    if (MP_OBJ_IS_INT(item)) {
        pixelbuf_set_pixel_int(buf, mp_obj_get_int_truncated(item), byteorder, bpp);
    } else {
        mp_obj_t *items;
        size_t len;
        mp_obj_get_array(item, &len, &items);
        if (len != bpp) {
            mp_raise_ValueError_varg("Expected tuple of length %d, got %d", bpp, len);
        }
        buf[pixelbuf_byteorder_lookup[byteorder].r] = mp_obj_get_int_truncated(items[PIXEL_R]);
        buf[pixelbuf_byteorder_lookup[byteorder].g] = mp_obj_get_int_truncated(items[PIXEL_G]);
        buf[pixelbuf_byteorder_lookup[byteorder].b] = mp_obj_get_int_truncated(items[PIXEL_B]);
        if (bpp > 3)
            buf[pixelbuf_byteorder_lookup[byteorder].w] = mp_obj_get_int_truncated(items[PIXEL_W]);
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


const mp_obj_type_t pixelbuf_pixelbuf_type = {
        { &mp_type_type },
        .name = MP_QSTR_PixelBuf,
        .subscr = pixelbuf_pixelbuf_subscr,
        .make_new = pixelbuf_pixelbuf_make_new,
        .unary_op = pixelbuf_pixelbuf_unary_op,
        .print = NULL,
        .locals_dict = (mp_obj_t)&pixelbuf_pixelbuf_locals_dict,
};
