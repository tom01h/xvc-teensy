#define BAUD_RATE 115200
#define UART_TX_PIN 0
#define UART_RX_PIN 1

typedef uint8_t cmd_buffer[512];
typedef struct buffer_info
{
  volatile uint32_t count;
  volatile uint8_t busy;
  cmd_buffer buffer;
} buffer_info;