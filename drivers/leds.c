#include "leds.h"

#include <mips/cpu.h>
#include <mfp_io.h>
static u32 led_status[3];

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

bool rt_leds_write_byte(char *v,u32 i){
   

   
}

bool rt_leds_write_by_num(char* buf,u32 num)//写我所申请的资源中的第num号，从0开始。2个：0号，1号。一次只写一个
{
    


}

bool rt_leds_require(u32 num)
{
    


}

bool rt_leds_release(u32 num)
{
    



}
