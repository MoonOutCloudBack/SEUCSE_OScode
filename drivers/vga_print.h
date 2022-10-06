#include <types.h>
#define PRINT_CTL_ADDR  0xB0A00000
#define SCREEN_W        64
#define SCREEN_H        24
#define SCREEN_MAX      SCREEN_W * SCREEN_H
#define TAB_W           4

int             cusor;          // cusor is the last position, 0 <= cusor < SCREEN_MAX

unsigned char   chMap[SCREEN_MAX];

void vga_print_init();

void vga_print_char(const char);

bool rt_vga_write(char * v);