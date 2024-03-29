#include "seven_seg.h"

#include <mips/cpu.h>
#include <mfp_io.h>

extern u32 get_asid();

static u32 seg_status[4]; // 记录数码管的状态

u32 get_seven_seg_enable() { return mips_get_word(SEVEN_SEG_EN_ADDR, NULL); }
void set_seven_seg_enable(u32 v) { mips_put_word(SEVEN_SEG_EN_ADDR, v); }
u32 get_seven_seg_value() { return mips_get_word(SEVEN_SEG_ADDR, NULL); }
void set_seven_seg_value(u32 v) { mips_put_word(SEVEN_SEG_ADDR, v); }

bool rt_segs_write(char * v){
    mips_put_word(SEVEN_SEG_ADDR, *((u32*)v));
    // 第一个 (u32*) 用来转指针的数据类型，第二个 *() 用来取指针指向的数值
}

bool rt_segs_read (char * v){ 
    *((u32*)v)=mips_get_word(SEVEN_SEG_ADDR, NULL); 
}

void init_seven_seg() {
    disable_all_seven_seg();
    clean_seven_seg_value();
    for (int i = 0; i < 4 ; i++)
        seg_status[i] = -1; // 全部置1
}

void enable_one_seven_seg(u32 pos) {
    if (pos < 8)
        set_seven_seg_enable(get_seven_seg_enable() | 1 << pos);
}
void enable_all_seven_seg() { set_seven_seg_enable(0xff); } // 高电平为 enable

void disable_one_seven_seg(u32 pos) {
    if (pos < 8)
        set_seven_seg_enable(get_seven_seg_enable() & ~(1 << pos));
}
void disable_all_seven_seg() { set_seven_seg_enable(0); }

void set_seven_seg_digit(u32 val, u32 pos) {
    if (val < 16 && pos < 8)
        set_seven_seg_value(get_seven_seg_enable() & ~(0xf << pos * 4) | val << pos * 4);
}

// 每个数码管的状态有8位， 4个共32位
// i：要写的数码管号
bool rt_segs_write_byte(char *v, u32 i){
    if(i>=4) return false;
    u32 tmp = (u32)*v;
    tmp = tmp << (i*8);
    u32 tmp2 = ~(0xff << (i * 8));

    // 令使能为 1
    set_seven_seg_enable( (3 << (i*2)) | get_seven_seg_enable() );  // 对指定的数码管使能
    // 写 sevenseg 的值
    set_seven_seg_value( tmp | (get_seven_seg_value() & tmp2) /* 保持其他数码管的值 */ );
    return true;
}

void clean_seven_seg_value() { set_seven_seg_value(0); }

bool rt_seven_seg_write_by_num(char* buf,u32 num){
    // TODO
}

/*以下两个函数用来模拟不同进程争夺四个临界资源的情况*/

bool rt_seven_seg_require(u32 num){
    u32 tmp = get_asid() & 0xff; // 为什么只保留末 8 位？
                                // RE：因为一个数码管的状态只有8位
    int count = 0;
    for(int i = 0; i < 4 && count != num; i++) {
        if (seg_status[i] == -1) {
            seg_status[i] = tmp;    // 设置为asid的末8位似乎只是一个被当前进程占有的标记
            count++;
        }
    }
    if(count == num) return true;
    return false;
}

bool rt_seven_seg_release(u32 num){    
    u32 tmp = get_asid() & 0xff;
    int count = 0;
    for(int i = 0; i < 4 && count != num; i++){
        if (seg_status[i] == tmp){
            seg_status[i] = -1;
            count++; 
        }
    }
    if(count == num) return true;
    return false;
}