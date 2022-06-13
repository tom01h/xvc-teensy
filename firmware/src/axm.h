#define PWD0_PIN 16
#define PWD1_PIN 17
#define PRD0_PIN 18
#define PRD1_PIN 19

#define PCK_PIN 26
#define PWAIT_PIN 27
#define PWRITE_PIN 25

#define AXM_ITF     0

#define axm_delay   10

// typedef uint8_t cmd_buffer[512];
// // [0]     W/R#
// // [2:1]   LEN (10bit)
// // [7:4]   ADDRESS
// // [511:8]  DATA or [511:0]

void pmod_task(void);