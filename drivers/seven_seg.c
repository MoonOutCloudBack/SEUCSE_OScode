#include "seven_seg.h"

#include <mips/cpu.h>
#include <mfp_io.h>
static u32 seg_status[4];
u32 get_seven_seg_enable() { return mips_get_word(SEVEN_SEG_EN_ADDR, NULL); }
void set_seven_seg_enable(u32 v) { mips_put_word(SEVEN_SEG_EN_ADDR, v); }
u32 get_seven_seg_value() { return mips_get_word(SEVEN_SEG_ADDR, NULL); }
void set_seven_seg_value(u32 v) { mips_put_word(SEVEN_SEG_ADDR, v); }

bool rt_segs_write(char * v){
    mips_put_word(SEVEN_SEG_ADDR, *((u32*)v)); 
}

bool rt_segs_read (char * v){ 
  *((u32*)v)=mips_get_word(SEVEN_SEG_ADDR, NULL); 
 }

void init_seven_seg() {
    disable_all_seven_seg();
    clean_seven_seg_value();
    for (int i = 0; i < 4 ; i++)
        seg_status[i] = -1;
}

void enable_one_seven_seg(u32 pos) {
    if (pos < 8)
        set_seven_seg_enable(get_seven_seg_enable() | 1 << pos);
}

void enable_all_seven_seg() { set_seven_seg_enable(0xff); }

void disable_one_seven_seg(u32 pos) {
    if (pos < 8)
        set_seven_seg_enable(get_seven_seg_enable() & ~(1 << pos));
}

void disable_all_seven_seg() { set_seven_seg_enable(0); }

void set_seven_seg_digit(u32 val, u32 pos) {
    if (val < 16 && pos < 8)
        set_seven_seg_value(get_seven_seg_enable() & ~(0xf << pos * 4) | val << pos * 4);
}

bool rt_segs_write_byte(char *v,u32 i){
    
}

void clean_seven_seg_value() { set_seven_seg_value(0); }
bool rt_seven_seg_write_by_num(char* buf,u32 num){
    
}

bool rt_seven_seg_require(u32 num){
    
}
bool rt_seven_seg_release(u32 num){    
    
}