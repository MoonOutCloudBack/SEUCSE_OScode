#include "vga_print.h"

void vga_print_init()
{
    for(int i = 0; i < SCREEN_MAX; ++i) chMap[i] = ' ';
    cusor = 0;
}

bool rt_vga_write(char * v) { }

void vga_print_char(const char ch) {

    // 1. 维护 chMap char 数组
    if(ch == '\n') { // 换行
        cusor = (cusor / SCREEN_W + 1) * SCREEN_W;
    }
    else if(ch == '\t') { // \t 制表
        // 好麻烦诶，干脆就作为 4 个字符吧
        cusor += 4;
    }
    else if(ch == '\r') ; // 不打算处理 \r 了，感觉都是 \n\r 一起出现的
    else { // 正常字符
        chMap[cusor] = ch; ++cusor;
    }
    
    // 2. 屏幕满了，那么上移一行吧
    if(cusor == SCREEN_MAX){
        for(int i = 1; i < SCREEN_H; ++i) {
            for(int j = 0; j < SCREEN_W; ++j) {
                chMap[i*SCREEN_W + j] = chMap[i*SCREEN_W + j - SCREEN_W];
            }
        }
        for(int j = 0; j < SCREEN_W; ++j) {
            chMap[SCREEN_MAX - SCREEN_W + j] = ' ';
        }
        cusor -= SCREEN_W;
    }
    
    // 3. 把字符串给 verilog
    for(int i = 0; i < 384; ++i) {
        // 一个 word 是一个 u32，我们要输出 24 行 * 64 列 * 8 char = 12288, / 32 = 384
        mips_put_word(PRINT_CTL_ADDR + i, *(u32 *) (chMap + 4*i) );
    }

    // volatile unsigned int * p = PRINT_CTL_ADDR;
}