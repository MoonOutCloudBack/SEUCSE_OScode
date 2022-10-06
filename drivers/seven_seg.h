#pragma once

#include <types.h>

u32 get_seven_seg_enable();
void set_seven_seg_enable(u32 v);
u32 get_seven_seg_value();
void set_seven_seg_value(u32 v);

void init_seven_seg();

void enable_one_seven_seg(u32 pos);

void enable_all_seven_seg();

void disable_one_seven_seg(u32 pos);

void disable_all_seven_seg();

void set_seven_seg_digit(u32 val, u32 pos);

void clean_seven_seg_value();

bool rt_segs_read (char * v);
bool rt_segs_write(char * v);
bool rt_segs_write_byte(char *v,u32 i);


bool rt_seven_seg_write_by_num(char* buf,u32 num);

bool rt_seven_seg_require(u32 num);
bool rt_seven_seg_release(u32 num);
