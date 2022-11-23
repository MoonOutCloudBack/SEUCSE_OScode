#include <types.h>
#define PRINT_CTL_ADDR  0xB0A00000
#define SCREEN_W        64
#define SCREEN_H        24
#define SCREEN_MAX      SCREEN_W * SCREEN_H
#define TAB_W           4

int             cusor;          // cusor 是下一个 char 要写的位置, 0 <= cusor < SCREEN_MAX

unsigned char   chMap[SCREEN_MAX];

void vga_print_init();

void vga_print_char(const char);

bool rt_vga_write(char * v);