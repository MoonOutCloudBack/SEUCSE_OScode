#include "leds.h"

#include <mips/cpu.h>
#include <mfp_io.h>

extern u32 get_asid();

static u32 led_status[3]; // 记录每组led的状态，初始化后为 -1（因为是无符号所以实际上是全1）

void led_init(){
    led_status[0] = -1;
    led_status[1] = -1;
    led_status[2] = -1;
    set_leds(0);
}
void set_leds(u32 v) { 
    mips_put_word(LEDS_ADDR, v); 
}

bool rt_leds_write(char * v) { 
    mips_put_word( LEDS_ADDR, *((u32*)v) ); 
}

bool rt_leds_read (char * v){ 
    *((u32*)v)=mips_get_word(LEDS_ADDR, NULL); 
}

bool rt_leds_write_byte(char *v, u32 i){
    if(i>2) return false; // 0 1 2，三组led
    u32 tmp = (u32)*v;
    tmp = tmp << (i*8);
    u32 tmp2 = ~(0xff << (i * 8));
    u32 ori = mips_get_word( LEDS_ADDR, NULL);
    mips_put_word( LEDS_ADDR, tmp | (ori & tmp2) );
    return true;
}

// 写我所申请的资源中的第num号，从0开始。2个：0号，1号。一次只写一个
bool rt_leds_write_by_num(char* buf, u32 num) 
{
    
    // TODO: 已经问了学长，这个函数用来做什么
    // 盲猜用来 1. require 一下，2. 写（buf 是数组），3. release

}

/*以下函数用于模拟函数之间争夺临界资源的场景*/
// 我需要一共 num 个资源
// 返回值表示是否能获取num个资源
bool rt_leds_require(u32 num)
{
    u32 tmp = get_asid() & 0xff;
    int count = 0;
    for(int i = 0; i < 3 && count != num; i++) {
        if (led_status[i] == -1) {
            led_status[i] = tmp;
            count++;
        }
    }
    if(count == num) return true;
    return false;
}

bool rt_leds_release(u32 num)
{
    u32 tmp = get_asid() & 0xff;
    int count = 0;
    for(int i = 0; i < 3 && count != num; i++){
        if (led_status[i] == tmp){
            led_status[i] = -1;
            count++; 
        }
    }
    if(count == num) return true;
    return false;
}
