/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 * Copyright (c) 2019 Artur Pacholec
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

#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/busio/UART.h"

#include "mpconfigport.h"
#include "lib/utils/interrupt_char.h"
#include "supervisor/shared/tick.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "periph.h"

#include "fsl_lpuart.h"

// TODO

#define UART_CLOCK_FREQ (CLOCK_GetPllFreq(kCLOCK_PllUsb1) / 6U) / (CLOCK_GetDiv(kCLOCK_UartDiv) + 1U)

static void config_periph_pin(const mcu_periph_obj_t *periph) {
    IOMUXC_SetPinMux(
        periph->pin->mux_reg, periph->mux_mode,
        periph->input_reg, periph->input_idx,
        0,
        0);

    IOMUXC_SetPinConfig(0, 0, 0, 0,
        periph->pin->cfg_reg,
        IOMUXC_SW_PAD_CTL_PAD_HYS(0)
            | IOMUXC_SW_PAD_CTL_PAD_PUS(0)
            | IOMUXC_SW_PAD_CTL_PAD_PUE(0)
            | IOMUXC_SW_PAD_CTL_PAD_PKE(1)
            | IOMUXC_SW_PAD_CTL_PAD_ODE(0)
            | IOMUXC_SW_PAD_CTL_PAD_SPEED(1)
            | IOMUXC_SW_PAD_CTL_PAD_DSE(6)
            | IOMUXC_SW_PAD_CTL_PAD_SRE(0));
}

void LPUART_UserCallback(LPUART_Type *base, lpuart_handle_t *handle, status_t status, void *user_data)
{
    busio_uart_obj_t *self = (busio_uart_obj_t*)user_data;

    if (status == kStatus_LPUART_RxIdle) {
        self->rx_ongoing = false;
    }
}

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
        const mcu_pin_obj_t * tx, const mcu_pin_obj_t * rx, uint32_t baudrate,
        uint8_t bits, uart_parity_t parity, uint8_t stop, mp_float_t timeout,
        uint16_t receiver_buffer_size) {

    // TODO: Allow none rx or tx

    bool have_tx = tx != mp_const_none;
    bool have_rx = rx != mp_const_none;
    if (!have_tx && !have_rx) {
        mp_raise_ValueError(translate("tx and rx cannot both be None"));
    }

    self->baudrate = baudrate;
    self->character_bits = bits;
    self->timeout_ms = timeout * 1000;

    const uint32_t rx_count = sizeof(mcu_uart_rx_list) / sizeof(mcu_periph_obj_t);
    const uint32_t tx_count = sizeof(mcu_uart_tx_list) / sizeof(mcu_periph_obj_t);

    for (uint32_t i = 0; i < rx_count; ++i) {
        if (mcu_uart_rx_list[i].pin != rx)
            continue;

        for (uint32_t j = 0; j < tx_count; ++j) {
            if (mcu_uart_tx_list[j].pin != tx)
                continue;

            if (mcu_uart_tx_list[j].bank_idx != mcu_uart_rx_list[i].bank_idx)
                continue;

            self->rx_pin = &mcu_uart_rx_list[i];
            self->tx_pin = &mcu_uart_tx_list[j];

            break;
        }
    }

    if(self->rx_pin == NULL || self->tx_pin == NULL) {
        mp_raise_RuntimeError(translate("Invalid UART pin selection"));
    } else {
        self->uart = mcu_uart_banks[self->tx_pin->bank_idx - 1];
    }

    config_periph_pin(self->rx_pin);
    config_periph_pin(self->tx_pin);

    lpuart_config_t config = { 0 };
    LPUART_GetDefaultConfig(&config);

    config.dataBitsCount = self->character_bits == 8 ? kLPUART_EightDataBits : kLPUART_SevenDataBits;
    config.baudRate_Bps = self->baudrate;
    config.enableTx = self->tx_pin != NULL;
    config.enableRx = self->rx_pin != NULL;

    LPUART_Init(self->uart, &config, UART_CLOCK_FREQ);

    claim_pin(self->tx_pin->pin);

    if (self->rx_pin != NULL) {
        ringbuf_alloc(&self->rbuf, receiver_buffer_size, true);

        if (!self->rbuf.buf) {
            LPUART_Deinit(self->uart);
            mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate RX buffer"));
        }

        LPUART_TransferCreateHandle(self->uart, &self->handle, LPUART_UserCallback, self);
        LPUART_TransferStartRingBuffer(self->uart, &self->handle, self->rbuf.buf, self->rbuf.size);

        claim_pin(self->rx_pin->pin);
    }
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->rx_pin == NULL && self->tx_pin == NULL;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }

    LPUART_Deinit(self->uart);

    gc_free(self->rbuf.buf);
    self->rbuf.size = 0;
    self->rbuf.iput = self->rbuf.iget = 0;

//    reset_pin_number(self->rx_pin);
//    reset_pin_number(self->tx_pin);

    self->rx_pin = NULL;
    self->tx_pin = NULL;
}

// Read characters.
size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    if (self->rx_pin == NULL) {
        mp_raise_ValueError(translate("No RX pin"));
    }

    if (len == 0) {
        // Nothing to read.
        return 0;
    }

    lpuart_transfer_t xfer = {
        .data = data,
        .dataSize = len,
    };

    self->rx_ongoing = true;
    LPUART_TransferReceiveNonBlocking(self->uart, &self->handle, &xfer, NULL);

    uint64_t start_ticks = supervisor_ticks_ms64();

    // Wait for all bytes received or timeout
    while (self->rx_ongoing && (supervisor_ticks_ms64() - start_ticks < self->timeout_ms) ) {
        RUN_BACKGROUND_TASKS;

        // Allow user to break out of a timeout with a KeyboardInterrupt.
        if (mp_hal_is_interrupted()) {
            break;
        }
    }

    // if we timed out, stop the transfer
    if (self->rx_ongoing) {
        LPUART_TransferAbortReceive(self->uart, &self->handle);
    }

    return len - self->handle.rxDataSize;
}

// Write characters.
size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    if (self->tx_pin == NULL) {
        mp_raise_ValueError(translate("No TX pin"));
    }

    LPUART_WriteBlocking(self->uart, data, len);

    return len;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    if (LPUART_SetBaudRate(self->uart, baudrate, UART_CLOCK_FREQ) == kStatus_Success) {
        self->baudrate = baudrate;
    }
}

mp_float_t common_hal_busio_uart_get_timeout(busio_uart_obj_t *self) {
    return (mp_float_t) (self->timeout_ms / 1000.0f);
}

void common_hal_busio_uart_set_timeout(busio_uart_obj_t *self, mp_float_t timeout) {
    self->timeout_ms = timeout * 1000;
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    return LPUART_TransferGetRxRingBufferLength(self->uart, &self->handle);
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
    self->handle.rxRingBufferHead = self->handle.rxRingBufferTail;
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    if (self->tx_pin == NULL) {
        return false;
    }

    return LPUART_GetStatusFlags(self->uart) & kLPUART_TxDataRegEmptyFlag;
}
