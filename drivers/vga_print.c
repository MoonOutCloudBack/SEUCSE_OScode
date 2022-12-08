#include "vga_print.h"

void vga_print_init()
{
    cusor = 0;
    int i = 0;
    for (i = 0; i < SCREEN_MAX; ++i) {
        chMap[i] = '\0';
    }
    volatile unsigned int *p = PRINT_CTL_ADDR;
    for (i = 0; i < SCREEN_MAX; ++i) {
        (*p) = ((int)i << 8); // 这种诡异的操作，就是 mips_put_char 嘛
        // 所谓高八位是 cursor
    }
}
bool rt_vga_write(char * v){
    while(*v!='\0'){
        vga_print_char(*v);
        v++;
    }
}
void vga_print_char(const char ch) {
    volatile unsigned int * p = PRINT_CTL_ADDR;
    if (ch == '\n') { // 换行
        int new_cusor = (cusor / SCREEN_W + 1) * SCREEN_W; // new cursor 是下一行最开始
        int i = 0;
        for (i = cusor; i < new_cusor && i < SCREEN_MAX; ++i) {
            chMap[i] = '\0'; // 上一行没写完的地方，都赋为 '\0'
            (*p) = (int)i << 8; // 准确的说，*p 被赋为高八位 cursor concat 低八位 '\0'
        }
        cusor = new_cusor;
    } else if (ch == '\t') {
        int new_cusor = (cusor / TAB_W + 1) * TAB_W; // cursor 跑去 4 的倍数
        int i = 0;
        for (i = cusor; i < new_cusor && i < SCREEN_MAX; ++i) {
            chMap[i] = '\0';
            (*p) = (int)i << 8; // 同样的，cursor | '\0'
        }
        cusor = new_cusor;
    } else if (ch == '\r') {
        cusor = (cusor / SCREEN_W) * SCREEN_W; // 回到本行最开始
    } else {
        (*p) = ((int)cusor << 8) | ((int)ch); // cursor | char
        chMap[cusor] = ch;
        ++cusor;
    }
    if (cusor >= SCREEN_MAX) {
        int i = 0;
        for (i = 0; i < SCREEN_MAX - SCREEN_W; ++i) {
            chMap[i] = chMap[i + SCREEN_W]; // chMap 整体上移一行
        }
        for (i = SCREEN_MAX - SCREEN_W; i < SCREEN_MAX; ++i) {
            chMap[i] = '\0'; // 最后一行空出来
        }
        cusor = cusor - SCREEN_W; // 回到上一行相同位置（其实就是最后一行最开始）
        for (i = 0; i < SCREEN_MAX; ++i) { // 一个一个 重新更新整屏的内容
            (*p) = ((int)i << 8) | ((int)chMap[i]);
        }
    }
}