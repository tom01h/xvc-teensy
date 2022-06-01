/*
  Copyright (c) 2017 Jean THOMAS.
  Copyright (c) 2021 Dhiru Kholia.

  Uses code from https://github.com/kholia/xvcpi/blob/master/xvcpi.c file.

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software
  is furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "bsp/board.h"
#include "fsl_gpio.h"

#include "xvcPico.h"
#include "tusb.h"
#include "jtag.h"

// Modified
enum CommandIdentifier {
  CMD_STOP = 0x00,
  CMD_XFER = 0x03,
  CMD_WRITE = 0x04,
};

static inline void gpio_write(uint32_t tck, uint32_t tms, uint32_t tdi)
{
  //gpio_put(tck_gpio, tck);
  //gpio_put(tms_gpio, tms);
  //gpio_put(tdi_gpio, tdi);
  uint32_t msk = (1ul << tck_gpio) | (1ul << tms_gpio) | (1ul << tdi_gpio);
  uint32_t set = (tck << tck_gpio) | (tms << tms_gpio) | (tdi << tdi_gpio);
  uint32_t clr = set ^ msk;
  GPIO4->DR_CLEAR = clr;
  GPIO4->DR_SET   = set;

  for (unsigned int i = 0; i < jtag_delay; i++)
    asm volatile("nop");
}

static inline int gpio_read(void)
{
  return GPIO_PinRead(GPIO4, tdo_gpio);
}

// Handler for "gpio_xfer" on the host side
static int cmd_xfer(int bitsLeft, const uint8_t *commands)
{
  cmd_buffer tx_buffer;

  int header_offset = 0;
  uint32_t n;
  int com_offset = 0;

  if(bitsLeft == 0){
    com_offset = 5;
    bitsLeft = (commands[4] << 24) | (commands[3] << 16) | (commands[2] << 8) | (commands[1] << 0);
    if(bitsLeft >= 252*8){
      n = 252*8;
    } else {
      n = bitsLeft;
    }
  } else {
    if(bitsLeft >= 256*8){
      n = 256*8;
    } else {
      n = bitsLeft;
    }
  }
  
  uint32_t bytes = (n + 7) / 8; // 252 or 256

  for (uint32_t j = 0; j < bytes; j++) {
    uint8_t tdo = 0;
    uint8_t tms = commands[j*2+com_offset];
    uint8_t tdi = commands[j*2+com_offset+1];
    if(((j + 1) != bytes) | ((n%8) == 0)){
      for (uint32_t i = 0; i < 8; i++) {
        gpio_write(0, tms & 1, tdi & 1);
        tms >>= 1;
        tdi >>= 1;
        tdo |= gpio_read() << i;
        GPIO4->DR_SET   = (1ul << tck_gpio);
      }
    } else {
      for (uint32_t i = 0; i < (n%8); i++) {
        gpio_write(0, tms & 1, tdi & 1);
        tms >>= 1;
        tdi >>= 1;
        tdo |= gpio_read() << i;
        GPIO4->DR_SET   = (1ul << tck_gpio);
      }
    }
    tx_buffer[header_offset++] = tdo;
  }

  /* Send the transfer response back to host */
  tud_vendor_n_write(JTAG_ITF, tx_buffer, bytes);
  tud_vendor_n_flush(JTAG_ITF);

  // debug code
  //board_led_write(false); // on

  return bitsLeft - n;
}

// Handler for "gpio_write" on the host side
static void cmd_write(const uint8_t *commands)
{
  uint8_t tck, tms, tdi;

  tck = commands[1];
  tms = commands[2];
  tdi = commands[3];
  gpio_write(tck & 1, tms & 1, tdi & 1);
}

void cmd_handle(uint8_t *rx_buf, __attribute__((unused)) uint32_t count)
{
  uint8_t *commands = (uint8_t*)rx_buf;
  static int bitsLeft = 0;

  if(bitsLeft != 0){
    bitsLeft = cmd_xfer(bitsLeft, commands);
    return;
  }

  while (*commands != CMD_STOP) {
    switch ((*commands) & 0x0F) {
      case CMD_XFER:
        bitsLeft = cmd_xfer(bitsLeft, commands);
        return;
        break;

      case CMD_WRITE:
        cmd_write(commands);
        commands += 3;
        break;

      default:
        return; /* Unsupported command, halt */
        break;
    }

    commands++;
  }

  return;
}