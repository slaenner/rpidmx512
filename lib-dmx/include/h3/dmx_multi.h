/**
 * @file dmx_multi.h
 *
 */
/* Copyright (C) 2018 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DMX_MULTI_H_
#define DMX_MULTI_H_

#include <stdint.h>

#include "dmx.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void dmx_multi_init(void);
extern void dmx_multi_init_set_gpiopin(uint8_t port, uint8_t gpio_pin);
extern void dmx_multi_set_port_direction(uint8_t port, _dmx_port_direction port_direction, bool enable_data);
extern void dmx_multi_set_port_send_data_without_sc(uint8_t uart, const uint8_t *data, uint16_t length);

extern uint32_t dmx_multi_get_output_break_time(void);
extern void dmx_multi_set_output_break_time(uint32_t);
extern uint32_t dmx_multi_get_output_mab_time(void);
extern void dmx_multi_set_output_mab_time(uint32_t);
extern uint32_t dmx_multi_get_output_period(void);

extern const uint8_t *dmx_multi_rdm_get_available(uint8_t uart);

#ifdef __cplusplus
}
#endif

#endif /* DMX_MULTI_H_ */
