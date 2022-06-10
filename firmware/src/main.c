/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
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
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "board.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_lpuart.h"

#include "xvcPico.h"
#include "jtag.h"
#include "axm.h"

void uart_task(void)
{
  char buf[512];

  if(tud_cdc_n_available(0)){
    uint32_t count = tud_cdc_n_read(0, buf, sizeof(buf));
    tud_cdc_n_read_flush(0);
    LPUART_WriteBlocking(UART_PORT, (uint8_t*)buf, count);
  }

  if(!(kLPUART_RxFifoEmptyFlag & LPUART_GetStatusFlags(UART_PORT))){
    LPUART_ReadBlocking(UART_PORT, (uint8_t*)buf, sizeof(buf));
    tud_cdc_n_write_str(0, buf);
    tud_cdc_n_write_flush(0);
  }
}

#define pin_config_val (IOMUXC_SW_PAD_CTL_PAD_DSE(2) | IOMUXC_SW_PAD_CTL_PAD_SPEED_MASK)

static inline void gpio_init( uint32_t muxRegister,
                              uint32_t muxMode,
                              uint32_t inputRegister,
                              uint32_t inputDaisy,
                              uint32_t configRegister,
                              uint32_t inputOnfield,    // IOMUXC_SetPinMux
                              uint32_t configValue,     // IOMUXC_SetPinConfig
                                                        // hw/mcu/nxp/mcux-sdk/devices/MIMXRT1062/MIMXRT1062.h
                              GPIO_Type *base,
                              uint32_t pin,
                              const gpio_pin_config_t *config)
{
  IOMUXC_SetPinMux   ( muxRegister, muxMode, inputRegister, inputDaisy, configRegister, inputOnfield);
  IOMUXC_SetPinConfig( muxRegister, muxMode, inputRegister, inputDaisy, configRegister, configValue);
  GPIO_PinInit       ( base, pin, config);
}

buffer_info buffer_info_jtag;
buffer_info buffer_info_axm;

void from_host_task(void)
{
  if ((buffer_info_jtag.busy == false))
  {
    tud_task();// tinyusb device task
    if (tud_vendor_n_available(JTAG_ITF))
    {
      uint count = tud_vendor_n_read(JTAG_ITF, buffer_info_jtag.buffer, 512);
      if (count != 0)
      {
        buffer_info_jtag.count = count;
        buffer_info_jtag.busy = true;
      }
    }
  }

  if ((buffer_info_axm.busy == false))
  {
    tud_task();
    if (tud_vendor_n_available(AXM_ITF))
    {
      uint count = tud_vendor_n_read(AXM_ITF, buffer_info_axm.buffer, 512);
      if (count != 0)
      {
        buffer_info_axm.count = count;
        buffer_info_axm.busy = true;
      }
    }
  }
}

void fetch_command(void)
{
  if (buffer_info_jtag.busy)
  {
    cmd_handle(buffer_info_jtag.buffer, buffer_info_jtag.count);
    buffer_info_jtag.busy = false;
  }
}

//this is to work around the fact that tinyUSB does not handle setup request automatically
//Hence this boiler plate code
/*bool tud_vendor_control_xfer_cb(__attribute__((unused)) uint8_t rhport, uint8_t stage, __attribute__((unused)) tusb_control_request_t const * request)
{
  if (stage != CONTROL_STAGE_SETUP) return true;
  return false;
}*/

int main(void)
{
  board_init();
  tusb_init();

  gpio_pin_config_t pout_config = { kGPIO_DigitalOutput, 0, kGPIO_NoIntmode };
  gpio_pin_config_t pin_config  = { kGPIO_DigitalInput,  0, kGPIO_NoIntmode };

  // JTAG init
  gpio_init( IOMUXC_GPIO_EMC_04_GPIO4_IO04, 0U, pin_config_val, GPIO4, tms_gpio, &pout_config); // TMS
  gpio_init( IOMUXC_GPIO_EMC_05_GPIO4_IO05, 0U, pin_config_val, GPIO4, tdi_gpio, &pout_config); // TDI
  gpio_init( IOMUXC_GPIO_EMC_06_GPIO4_IO06, 0U, pin_config_val, GPIO4, tdo_gpio, &pin_config ); // TDO
  gpio_init( IOMUXC_GPIO_EMC_08_GPIO4_IO08, 0U, pin_config_val, GPIO4, tck_gpio, &pout_config); // TCK

  GPIO_PinWrite(GPIO4, tms_gpio, 1);
  GPIO_PinWrite(GPIO4, tdi_gpio, 0);
  GPIO_PinWrite(GPIO4, tck_gpio, 0);

  // Set up our UART with the required speed.
  
  // pmod init
  gpio_init( IOMUXC_GPIO_AD_B1_00_GPIO1_IO16, 0U, pin_config_val, GPIO1, PWD0_PIN, &pout_config); // PWD0
  gpio_init( IOMUXC_GPIO_AD_B1_01_GPIO1_IO17, 0U, pin_config_val, GPIO1, PWD1_PIN, &pout_config); // PWD1
  gpio_init( IOMUXC_GPIO_AD_B1_02_GPIO1_IO18, 0U, pin_config_val, GPIO1, PRD0_PIN, &pin_config);  // PRD0
  gpio_init( IOMUXC_GPIO_AD_B1_03_GPIO1_IO19, 0U, pin_config_val, GPIO1, PRD1_PIN, &pin_config);  // PRD1

  gpio_init( IOMUXC_GPIO_AD_B1_10_GPIO1_IO26, 0U, pin_config_val, GPIO1, PCK_PIN, &pout_config);  // PCK
  gpio_init( IOMUXC_GPIO_AD_B1_11_GPIO1_IO27, 0U, pin_config_val, GPIO1, PWAIT_PIN, &pin_config); // PWAIT
  gpio_init( IOMUXC_GPIO_AD_B1_09_GPIO1_IO25, 0U, pin_config_val, GPIO1, PWRITE_PIN, &pout_config); // PWRITE

  GPIO_PinWrite(GPIO1, PWD0_PIN, 0);
  GPIO_PinWrite(GPIO1, PWD1_PIN, 0);
  GPIO_PinWrite(GPIO1, PCK_PIN, 0);
  GPIO_PinWrite(GPIO1, PWRITE_PIN, 0);
  
  // LED config (board_init)
  // gpio_init( LED_PINMUX, 0U, 0x10B0U, LED_PORT, LED_PIN, &pout_config);
  // board_led_write(true);
  
  buffer_info_jtag.busy = false;
  buffer_info_axm.busy = false;

  while (1)
  {
    from_host_task();
    fetch_command();
    pmod_task();
    //uart_task();

  }

  return 0;
}