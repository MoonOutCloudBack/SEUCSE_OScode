#include "vga_print.h"

void vga_print_init()
{
    
}
bool rt_vga_write(char * v){
   return true;
}
void vga_print_char(const char ch) {
    volatile unsigned int * p = PRINT_CTL_ADDR;
    
}