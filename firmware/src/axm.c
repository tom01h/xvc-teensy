#include "bsp/board.h"
#include "fsl_gpio.h"

#include "tusb.h"
#include "xvcPico.h"
#include "axm.h"

extern buffer_info buffer_info_axm;

uint32_t msk = (1 << PCK_PIN) | (1 << PWRITE_PIN) | (3  << PWD0_PIN);

void plen(int c, bool w)
{
  for(int j=0; j < 5; j++)
  {
    uint32_t set = (1 << PCK_PIN) | (w << PWRITE_PIN) | ((c&3) << PWD0_PIN);
    uint32_t clr = set ^ msk;
    GPIO1->DR_SET   = set;
    GPIO1->DR_CLEAR = clr;
    for(int k=0; k < axm_delay; k++)
      asm volatile("nop");
    c >>= 2;
    GPIO1->DR_CLEAR = (1 << PCK_PIN);
    for(int k=0; k < axm_delay; k++)
      asm volatile("nop");
  }
}

void pread(uint8_t *buffer, int size)
{
  for(int i=0; i < size; i++)
  {
    uint8_t c = 0;
    uint32_t ci;
    for(int j=0; j < 4; )
    {
      GPIO1->DR_SET   = (1 << PCK_PIN);
      for(int k=0; k < axm_delay; k++)
        asm volatile("nop");
      c >>= 2;
      j++;
      ci =  (uint32_t)(GPIO1->DR);
      GPIO1->DR_CLEAR = (1 << PCK_PIN);
      for(int k=0; k < axm_delay; k++)
        asm volatile("nop");
      if(!(ci & (1 << PWAIT_PIN))){
        c |= (ci >> (PRD1_PIN - 7)) & 0xc0;
      } else {
        c <<= 2;
        j--;
      }
    }
    buffer[i] = c;
  }
}

void pwrite(uint8_t *buffer, int size, bool w)
{
  for(int i=0; i < size; i++)
  {
    uint8_t c = buffer[i];
    for(int j=0; j < 4; j++)
    {
      uint32_t set = (1 << PCK_PIN) | (w << PWRITE_PIN) | ((c&3) << PWD0_PIN);
      uint32_t clr = set ^ msk;
      GPIO1->DR_SET   = set;
      GPIO1->DR_CLEAR = clr;
      for(int k=0; k < axm_delay; k++)
        asm volatile("nop");
      c >>= 2;
      GPIO1->DR_CLEAR = (1 << PCK_PIN);
      for(int k=0; k < axm_delay; k++)
        asm volatile("nop");
    }
  }
}

void pmod_task()
{
  static int write;
  static int size;
  int len;
  int max_wlen;
  int max_rlen = 512;
  uint8_t *buffer;
  if (buffer_info_axm.busy){
    if(size == 0){
      write = buffer_info_axm.buffer[0];
      len = buffer_info_axm.buffer[2] * 256 + buffer_info_axm.buffer[1];
      switch(len){
        case 1:  size = 1; break;
        case 2:  size = 2; break;
        case 4:  size = 4; break;
        case 6:
        case 7:  size = 8; break;
        default: size = len + 8; break;
      }
      plen(len, write);                              // LEN
      pwrite(&buffer_info_axm.buffer[4], 4, write);  // ADDRESS
      buffer = &buffer_info_axm.buffer[8];
      max_wlen = 512-8;
    }else{
      buffer = &buffer_info_axm.buffer[0];
      max_wlen = 512;
    }
    for(int i=0; i < axm_delay; i++)
      asm volatile("nop");
    if(write){               // WRITE
      if(size > max_wlen){                           // DATA
        pwrite(buffer, max_wlen, 1);
        size -= max_wlen;
      }else{
        pwrite(buffer, size, 1);
        size = 0;
      }
      buffer_info_axm.busy = false;
    }else{                   // READ
      while(size){
        if(size > max_rlen){                         // DATA
          pread(&buffer_info_axm.buffer[0], max_rlen);
          buffer_info_axm.count = max_rlen;
          size -= max_rlen;
        }else{
          pread(&buffer_info_axm.buffer[0], size);
          buffer_info_axm.count = size;
          buffer_info_axm.busy = false;
          size = 0;
        }
        while(tud_vendor_write(buffer_info_axm.buffer, buffer_info_axm.count) == 0) tud_task();
        tud_vendor_flush();
      }
    }
  }
}
