/*
 * asm.h: Assembler macros to make things easier to read.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1998 Ralf Baechle
 */

#include "regdef.h"
#include "cp0regdef.h"

/*
 * LEAF - declare leaf routine
 # LEAF ROUTINE 应该指的是由用户函数调用的函数，而并非最外层函数，因此返回地址总是存在ra中
 */
#define LEAF(symbol)                                    \
                .globl  symbol;                         \
                .align  2;                              \
                .type   symbol,@function;               \
                .ent    symbol,0;                       \
symbol:         .frame  sp,0,ra

/*
 * NESTED - declare nested routine entry point
 * .globl symbol意为symbol全局可见
 * .align n 将下一个数据与2的n次方对齐
 * .type name,@type:将符号name的type属性设为type。其中type可以是function或object
 * .ent, .end分别用来标志函数的开头和结尾
 * .symbol用于指定栈顶指针寄存器(sp)，该函数分配的栈大小，以及返回值寄存器
 * 由于此类函数结束后进程结束，因此返回地址由自己指定
 */
#define NESTED(symbol, framesize, rpc)                  \
                .globl  symbol;                         \
                .align  2;                              \
                .type   symbol,@function;               \
                .ent    symbol,0;                       \
symbol:         .frame  sp, framesize, rpc


/*
 * END - mark end of function
 */
#define END(function)                                   \
                .end    function;                       \
                .size   function,.-function


