

/*
 * boot.S for microAptiv_UP
 *
 *  Created on: April 12, 2014
 *  Author: MIPS TECHNOLOGIES, INC
 *  Start of boot code
*/
/*
Copyright (c) 2014, Imagination Technologies LLC and Imagination Technologies
Limited.
All rights reserved.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions in binary form must be built to execute on machines
   implementing the MIPS32(R), MIPS64 and/or microMIPS instruction set
   architectures.
2. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
3. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
4. Neither the name of Imagination Technologies LLC, Imagination Technologies Limited
   nor the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL IMAGINATION TECHNOLOGIES LLC OR IMAGINATION
TECHNOLOGIES LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#define _RESETCODE 1

#include <mips/regdef.h>
#include <mips/asm.h>
#include <asm/cp0regdef.h>
#include "boot.h"
#include "mfp_io.h"
#include <m32c0.h>

.set    noreorder               # Don't allow the assembler to reorder instructions.
# 解释：默认情况下，MIPS 汇编器会尝试重新排序周围的指令，自动填充分支、加载延迟槽。
# 但是，如果我们需要保持对指令顺序的精确控制，比如在大量使用的库函数的情况下，那么 .set noreorder
# 它用来告诉汇编器，在下次遇到相应的命令之前，抑制其重新排序功能

    # The first function, __reset_vector, just loads the address of the __cpu_init function
    # and jumps to it. This does 2 things; First it will jump to a KSEG1 address which is a
    # mirror of the BEV�s KSEG0 address but cacheable. Second it will do a mode switch to
    # enable micromips mode.

/**************************************************************************************
    R E S E T   E X C E P T I O N   H A N D L E R
**************************************************************************************/

LEAF(__reset_vector) # leaf 貌似是（不调用其他函数的）函数的意思
    la      a2, __cpu_init   # load address
    jr      a2               # pc <- a2, a0-a3 是 function call 的 first four parameters
    mtc0    zero, C0_COUNT   # Clear cp0 Count (Used to measure boot time.)
	# MIPS32 定义了 2 条协处理器访问指令：mtc0 实现修改 CP0 中的寄存器，mfc0 实现读取 CP0 中的寄存器。
	# mtc0: mtc0 arg1,arg2; move to CP0, arg2 CP0 寄存器 <= arg1 通用寄存器的值
	# mfc0: mfc0 arg1,arg2; move from CP0, arg1 通用寄存器 <= arg2 CP0 寄存器的值
    nop

END(__reset_vector)

LEAF(__cpu_init)

    # Verify the code is here due to a reset and not NMI. If this is an NMI then trigger
    # a debugger breakpoint using a sdbp instruction.

    # NMI 是什么：https://blog.csdn.net/fervor_heart/article/details/8288914
    # Non Maskable Interrupt，不可屏蔽中断
    # 无论状态寄存器中 IE 位（interrput enable，1 为 enable）如何, CPU 收到有效的 NMI 必须响应，不能屏蔽
    # 通常用于故障处理（协处理器运算出错, 存储器校验出错, I/O 通道校验出错等）

    mfc0    t0, CP0_STATUS          # Read CP0 Status, t 寄存器可以随便用
    ext     t0, t0, 19, 1           # extract NMI. NMI 位：1 表示是不可屏蔽中断，软件只能对其写入 0
	# ext 指令： ext rD,rS,pos,size：  rD <= rS[pos+size-1:pos]。 extract bit field，提取一部分 bits，
    beqz    t0, init_resources      # Branch if this is NOT an NMI exception.
    nop
    sdbbp                           # Failed assertion: NMI. 那么，现在要处理 NMI 中断了
    # sdbbp 指令：产生 EJTAG 异常的断点指令。如果执行遇到 sdbbp，则触发调试异常（断点）


init_resources:                 # initializes resources for "cpu".

    # Initialize CP0 registers
   
   # la      t0, init_cp0        # Init CP0 Status, Count, Compare, Watch*, and Cause.
   # jr      t0

   jal init_cp0
    nop

    # Initialize the TLB
 
   # la      t0, init_tlb        # Generate unique EntryHi contents per entry pair，以防 tlb 重复项而关闭
   # jr      t0
   
    jal init_tlb
    nop

    #Initialize the Instruction cache

    # la      a2, init_icache     # Initialize the L1 instruction cache. (Executing using I$ on return.)
    # jalr    a2
    # nop


    # The changing of Kernel mode cacheability must be done from KSEG1
    # Since the code is executing from KSEG0 It needs to do a jump to KSEG1 change K0 and jump back to KSEG0
    
    # ########## MIPS 的存储空间 #############
    # MIPS 将存储空间划分为4大块: kuseg, kseg0, kseg1, kseg2.

    # 0xFFFF FFFF
    # mapped kseg2, 放置内核模块 (同内核代码是分开放的，代码在 Kseg0)
    # 0xC000 0000
    # unmapped uncached kseg1, 用于运行 Bootloader，或映射 I/O 寄存器
    # 0xA000 0000
    # unmapped cached kseg0, 用于运行 OS 核心程序，异常入口点
    # 0x8000 0000
    # 2G kuseg, 主要用于 MMU 映射 / 运行用户程序。等到 MMU TLB 初始化后, 才可以存取
    # 0x0000 0000

    # 注意，kseg0 和 kseg1 都是映射上物理地址 0x00000000 - 0x1fffffff 上
    # 因此，程序运行的起始地址 0xbfc00000 实际上是物理地址 0x1fc00000
    # #######################

    la          t0, change_k0_cca   # load address，change_k0_cca 在 init_cache 里
    li          t1, 0x1             # load immediate
    ins         t0, t1, 29, 1       # changed to KSEG1 address by setting bit 29
    # ins 指令：rD, rS, pos, size;  insert，rD[P+S-1:P] = rS[S-1:0]
    # 在这里，表示将 change_k0_cca 第 29 位, 置为 1
    jr          t0
    nop

    # Initialize the Data cache

    # la      a2, init_dcache     # Initialize the L1 data cache
    # jalr    a2
    # nop
    

    # Prepare for eret to main.

    la      ra, all_done        # If main returns then go to all_done. 寄存器 ra 是当前过程的 return addr
    move    a0, zero            # Indicate that there are no arguments available.
    # move 指令：move rD, rS: rD <- rS。此处将 a0 置为 0，用来表示没有 arguments 可用
    la      v0, _start          # load the address of entry point _start.
    # 寄存器规则：v0 v1 是 function 的 return
    mtc0    v0, CP0_ERROREPC    # Write ErrorEPC with the address of _start
    # 重申：mtc0 的两个参数中，是前者把值给后者
    ehb                         # clear hazards (makes sure write to ErrorPC has completed)
    # ehb 指令：exception hazard barrier，功效为插入足够多的 nop，
    # 用来保证, 在运行后面代码之前，前面的内容已经安全写入寄存器了（规避流水冒险）
    # 类似指令：eret，从异常（原子的）返回；jr.hb jalr.hb，跳转寄存器指令，用来消除指令危险


    # Return from exception will now execute code in main
    eret                        # Exit reset exception handler and start execution of _start.
    # eret 指令：exception return，使用 ErrorEPC 寄存器作为跳转的 PC（而非 EPC）
    # 背景知识：MIPS 的异常程序计数器 (excepTIon program counter,EPC) 属于 CP0 寄存器, 用于保存造成异常的那条指令的地址
    # 所以，处理完异常，就跳回 EPC / ErrorEPC 继续执行
    

/**************************************************************************************/
all_done:
    # If main returns it will return to this point.  Just spin here.
    j       all_done
    # j 指令：PC = PC[31:28] | 后面的 28 位地址。这里表示 spin waiting
    # （另一种说法是这里用 b，不过 1. 不知道是否正确以及副作用，2. 感觉 j 至少没错）
    # ## RE: b的操作数是偏移，需要计算当前PC值和all_done地址的差，容易算错，不如j
    nop

END(__cpu_init)

# Inline the code: fill the rest of space between here and the next exception vector address.

#include "init_caches.S"

/**************************************************************************************
    B O O T   E X C E P T I O N   H A N D L E R S (CP0 Status[BEV] = 1)
**************************************************************************************/
/* NOTE: the linker script must insure that this code starts at start + 0x200 so the exception */
/* vectors will be addressed properly. All .org assume this! */
/* TLB refill, 32 bit task. */
.org 0x200                      # TLB refill, 32 bit task.
    sdbbp                       # This has the effect of starting the debugger
    nop

.org 0x280                      # XTLB refill, 64 bit task. start + 0x280
    sdbbp                       # This has the effect of starting the debugger
    nop

#include "init_cp0.S"

.org 0x300                      # Cache error exception. start + 0x300
    sdbbp                       # This has the effect of starting the debugger
    nop

#include "init_tlb.S"


.org 0x380    /* General exception. */
    # display alternating one-zeros on board LEDs
    li      t0, LEDS_ADDR           # Board LEDs display
    li      t1, 0xaaaaaa            # alternating one-zeros。我们有 24 个 LED 灯, 6 个 1010 应该足够了
    sw      t1, 0(t0)               # Write pattern to LEDs
    # sw 指令：sw rS, offset16(rT):  mem32(rT+offset16) <- rS
    # 32，16：一下子 write 内存 32 位，仅取 offset 低 16 位
    eret
    nop

# If you want the above code to fit into 1k flash you will need to leave out the
# code below. This is the code that covers the debug exception which you normally will not get.

.org 0x480 /* EJTAG debug exception (EJTAG Control Register[ProbTrap] == 0.) */
    li      r24_malta_word, LEDS_ADDR     # If post boot then $24 is clobbered.
    mtc0    a0, C0_DESAVE       # DeSave a0
    mfc0    a0, C0_DEPC         # Read DEPC
    sw      a0, 0(r24_malta_word)    # Display lower 16 bits of DEPC if there is not an EJTAG probe.
    mfc0    a0, C0_DESAVE       # Restore a0
1:  b       1b  /* Stay here */
    nop
