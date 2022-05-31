#define tms_gpio     4
#define tdi_gpio     5
#define tdo_gpio     6
#define tck_gpio     8

#define JTAG_ITF     1

#define jtag_delay   1
// #define jtag_delay   5
// #define jtag_delay   200

void cmd_handle(uint8_t *rx_buf, __attribute__((unused)) uint32_t count);