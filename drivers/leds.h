#pragma once

#include <types.h>

void set_leds(u32 v);
bool rt_leds_write(char *);

bool rt_leds_read (char *);
bool rt_leds_write_byte(char *v,u32 i);
bool rt_leds_write_by_num(char* buf,u32 num);

void led_init();
bool rt_leds_require(u32 num);
bool rt_leds_release(u32 num);
