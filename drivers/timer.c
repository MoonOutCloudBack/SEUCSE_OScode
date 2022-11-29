#include "timer.h"

#include <mips/cpu.h>
#include <mfp_io.h>
#include <printf.h>
#include "seven_seg.h"

// CPU 给的时钟信号是 50MHz

#define AXI_CLOCK_PERIOD_HZ     50000000
#define AXI_CLOCK_PERIOD_KHZ    50000

// Register Address Map                // Description
#define TCSR0_ADDR (0x00 + TIMER_ADDR) // Timer 0 Control and Status Register
#define TLR0_ADDR  (0x04 + TIMER_ADDR) // Timer 0 Load Register
#define TCR0_ADDR  (0x08 + TIMER_ADDR) // Timer 0 Counter Register
#define TCSR1_ADDR (0x10 + TIMER_ADDR) // Timer 1 Control and Status Register
#define TLR1_ADDR  (0x14 + TIMER_ADDR) // Timer 1 Load Register
#define TCR1_ADDR  (0x18 + TIMER_ADDR) // Timer 1 Counter Register

// 这 12 位就是 control & status 寄存器的低 12 位约定
#define TIMER_TCSR0_CASC    (1 << 11)   // Enable cascade mode of timers
#define TIMER_TCSR0_ENALL   (1 << 10)   // Enable All Timers
#define TIMER_TCSR0_PWMA0   (1 << 9)    // Enable Pulse Width Modulation for Timer 0
#define TIMER_TCSR0_T0INT   (1 << 8)    // Timer 0 Interrupt
#define TIMER_TCSR0_ENT0    (1 << 7)    // Enable Timer 0
#define TIMER_TCSR0_ENIT0   (1 << 6)    // Enable Interrupt for Timer 0
#define TIMER_TCSR0_LOAD0   (1 << 5)    // Load Timer 0
#define TIMER_TCSR0_ARHT0   (1 << 4)    // Auto Reload/Hold Timer 0
#define TIMER_TCSR0_CAPT0   (1 << 3)    // Enable External Capture Trigger Timer 0
#define TIMER_TCSR0_GENT0   (1 << 2)    // Enable External Generate Signal Timer 0
#define TIMER_TCSR0_UDT0    (1 << 1)    // Up/Down Count Timer 0
#define TIMER_TCSR0_MDT0    (1 << 0)    // Timer mode is generate

// TCSR 是 Timer Control and Status Register 的意思
// TLR 是 Timer Load Register 的意思
// TCR 是 Timer Counter Register 的意思

void set_TCSR0(u32 v) { mips_put_word(TCSR0_ADDR, v); }
u32 get_TCSR0() { return mips_get_word(TCSR0_ADDR, NULL); }
void set_TLR0(u32 v) { mips_put_word(TLR0_ADDR, v); }
u32 get_TLR0() { return mips_get_word(TLR0_ADDR, NULL); }
u32 get_TCR0() { return mips_get_word(TCR0_ADDR, NULL); }

void set_TCSR1(u32 v) { mips_put_word(TCSR1_ADDR, v); }
u32 get_TCSR1() { return mips_get_word(TCSR1_ADDR, NULL); }
void set_TLR1(u32 v) { mips_put_word(TLR1_ADDR, v); }
u32 get_TLR1() { return mips_get_word(TLR1_ADDR, NULL); }
u32 get_TCR1() { return mips_get_word(TCR1_ADDR, NULL); }

bool get_timer0_int() { return (get_TCSR0() & TIMER_TCSR0_T0INT) != 0; } // timer 0 发生了 interrupt
void clear_timer0_int() { set_TCSR0(get_TCSR0() | TIMER_TCSR0_T0INT); } // 清空本次 interrupt

void enable_timer0() { set_TCSR0(get_TCSR0() | TIMER_TCSR0_ENT0); }
void disable_timer0() { set_TCSR0(get_TCSR0() & ~(TIMER_TCSR0_ENT0)); }

void enable_timer1() { set_TCSR1(get_TCSR1() | TIMER_TCSR0_ENT0); }
void disable_timer1() { set_TCSR1(get_TCSR1() & ~(TIMER_TCSR0_ENT0)); }

void set_timing_interval_s(u32 s) {
    if (s > 85) {
        panic("Timeing interval cannot be over 85 seconds.\n");
        return;
    }
    set_TLR0(s * AXI_CLOCK_PERIOD_HZ - 4);
}

void set_timing_interval_ms(u32 ms) {
    if (ms > 85899) { // 是的，我们一共能计时 85.89934592 秒
        panic("Timeing interval cannot be over 85899 milliseconds.\n");
        return;
    }
    set_TLR0(ms * AXI_CLOCK_PERIOD_KHZ - 4);
}

void set_timing_interval_ms_for_timer1(u32 ms) {
    if (ms > 85899) {
        panic("Timeing interval cannot be over 85899 milliseconds (timer 1).\n");
        return;
    }
    set_TLR1(ms * AXI_CLOCK_PERIOD_KHZ - 4);
}

extern void set_exl();

void init_timer() {
    // 应该是 generate mode 吧

    // 1. 初始化
    set_TCSR0(0);
    set_TCSR1(0);
    set_exl();

    // 2. 设中断时长
    // set_timing_interval_s(1);
    set_timing_interval_ms(1000); // 中断时间

    // 3. 设 timer 工作模式
    // 文档：https://docs.xilinx.com/v/u/en-US/pg079-axi-timer
    set_TCSR0(
        TIMER_TCSR0_ENIT0 | // 开 timer 0 中断
        TIMER_TCSR0_ARHT0 | // 开 Auto Reload
        TIMER_TCSR0_UDT0    // 选择 down counter（从全 0 滚动到全 1）
    );                      // 以及 generate mode
    // set_TCSR1(
    //     TIMER_TCSR0_ARHT0 | // 开 Auto Reload
    //     TIMER_TCSR0_UDT0    // 选择 down counter（从全 0 滚动到全 1）
    // );                      // 以及不开 timer 1 中断, 选 generate mode
    // set_timing_interval_ms_for_timer1(1000);

    // 4. 开启 timer
    enable_timer0();
    // enable_timer1();
}


// misc: 如果启用 GenerateOut（TCSR 中的位 GENT），将产生一个输出脉冲（宽度为一个时钟周期）。