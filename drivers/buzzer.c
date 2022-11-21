#include "buzzer.h"

#include <mips/cpu.h>
#include <mfp_io.h>

void set_buzzers(u32 v)
{
    mips_put_word(BUZZER_ADDR,v);
}
void delay_zero()
{
    volatile unsigned int j = 0;
    for (; j < 2500000; j++);

}
void delay_do()
{
    volatile unsigned int j = 0;
    for (; j < 2500000; j++)
        set_buzzers(1);
    set_buzzers(0);
}
void delay_re()
{
    
volatile unsigned int j = 0;
    for (; j < 2500000; j++)
        set_buzzers(2);
    set_buzzers(0);
}
void delay_mi()
{
   volatile unsigned int j = 0;
    for (; j < 2500000; j++)
        set_buzzers(3);
    set_buzzers(0);
}
void delay_long_mi()
{
    volatile unsigned int j = 0;
    for (; j < 5000000; j++)
        set_buzzers(3);
    set_buzzers(0);
}
void delay_fa()
{
    volatile unsigned int j = 0;
    for (; j < 2500000; j++)
        set_buzzers(4);
    set_buzzers(0);
}
void delay_so()
{
   volatile unsigned int j = 0;
    for (; j < 2500000; j++)
        set_buzzers(5);
    set_buzzers(0);
}
void delay_la()
{
    volatile unsigned int j = 0;
    for (; j < 2500000; j++)
        set_buzzers(6);
    set_buzzers(0);
}
void delay_xi()
{
    volatile unsigned int j = 0;
    for (; j < 2500000; j++)
        set_buzzers(7);
    set_buzzers(0);
}

//0 
//1 262 0x00000106
//2 294 0x00000126
//3 330 0x0000014A
//4 349 0x0000015D
//5 392 0x00000188
//6 440 0x000001B8
//7 494 0x000001EE

void start_ringtone()
{
    //SONG OF JOY
    delay_xi();
    delay_zero();
    delay_mi();
    delay_mi();
    delay_fa();
    delay_so();
    delay_zero();
    delay_so();
    delay_fa();
    delay_mi();
    delay_re();
    delay_zero();
    delay_do();
    delay_do();
    delay_re();
    delay_mi();
    delay_zero();
    delay_long_mi();
    delay_re();
    delay_re();
}

void boot_music()
{
    //1233 5661 1321 123
    delay_do();
    delay_mi();
    delay_re();
    delay_mi();
    delay_mi();
    delay_so();
    delay_la();
    delay_la();
    delay_do();
    delay_zero();
    delay_do();
    delay_mi();
    delay_re();
    delay_do();
    delay_do();
    delay_re();
    delay_mi();

}
