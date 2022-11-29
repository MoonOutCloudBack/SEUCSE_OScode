/*
 * init_tlb.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Common TLB initialization for MIPS cores
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

#define _BOOTCODE 1

#include "boot.h"
#include <mips/regdef.h>
#include <mips/asm.h>
#include <mips/m32c0.h>

	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.



# TLB：用于缓存虚拟地址（以及它的 ASID）+ 其映射的物理地址（内存里）。
	# 具体的，我们只需要知道某个虚拟地址的 页（4KB）在物理存储的什么地址。
	# 通过将 VA 的高位（连同 ASID 位）与 TLB 每个条目的 tag 进行比较，来执行转换。

# ASID：address space identifier
	# 问题背景：
		# 每个进程都有自己的虚拟地址空间。假设它们都想在虚拟地址 0x0000 0000 放数据，这个数据要放到内存上（物理存储）
		# 然而，不同进程的 0x0000 0000 数据，不一定放在相同的物理地址
		# 所以在 TLB 中，可能存在多个 VA = 0x0000 0000，但 PA 不同的 entry
		# 为了避免这种情况，可以在每次 context switch（切换进程）时，把 TLB flush（冲掉、清除）一下。
		# 然而 MIPS 有其他解决方案：
	# 在 MIPS 中，每个 TLB entry 有一个 ASID，标识该 entry 属于哪个进程。同时，CP0_EntryHI 中的 ASID 是当前进程的 ASID
	# 所以进程查询 TLB 时，即使 VPN 命中，但如果该表项不是 global、且 ASID 与 CP0_EntryHi 的 ASID 不一致，则也视作 TLB 缺失
	# 这样就不用每次切换进程，都 flush 掉所有 tlb
	# 参考资料：
		# https://www.cnblogs.com/Melles/p/10714224.html
		# https://blog.csdn.net/Am_111/article/details/8696915

# MIPS 的 TLB 包括一个 joint TLB (JTLB)、一个小的 instruction TLB (ITLB)、一个小的 Data TLB (DTLB)
	# ITLB DTLB 由硬件实现，对软件透明，我们可以不考虑它
	# （不重要的细节）JTLB 采取某种“双重”（dual）组织方式，每个 tag entry 对应两个 V->P 映射，分别为 偶数 奇数

# JTLB 相关指令：（都没有参数）
	# tlbr：读 TLB entry。用 index 寄存器所指向的 TLB entry，填充 CP0 的 EntryHi、EntryLo0、EntryLo1、PageMask
	# tlbwi：通过 index 寄存器，写 TLB entry。把 CP0 的上面四个寄存器的信息，写到 index 寄存器所指向的 TLB entry
	# tlbwr：通过 random 寄存器，写 TLB entry。把 CP0 的上面四个寄存器的信息，写到 index 寄存器所指向的 TLB entry
		# 帮助实现随机替换策略
	# 看起来，EntryHi、EntryLo0、EntryLo1、PageMask，它们和 cache 那边 TagLo DataLo 的作用一样，也是缓冲

# 在使用 tlb 指令前，需要先用马上要写的这个 entry 的信息，来更新某些 CP0 寄存器:
	# PageMask: PageMask
	# VPN2, VPN2X, ASID: EntryHi
	# PFN0, C0, D0, V0, G: EntryLo0
	# PFN1, C1, D1, V1, G: EntryLo1
	# 注意，EntryLo0 EntryLo1 里面都有 G，真正的 G 值是它们俩 and 起来

# 初始化 TLB 的注意事项：
	# 如果两个 entry 对应同一个 VA，可能会出现 “TLB Shutdown”
	# 所以，要给每个 entry 赋值不同的 VA，建议以 8K 作为间隔


/**************************************************************************************
**************************************************************************************/
LEAF(init_tlb)

check_for_tlb:
	# Determine if we have a TLB
	mfc0    t0, C0_CONFIG			# read C0_Config，存在 t0

	# C0_CONFIG1 寄存器的 MT（9:7）表示 MMU type：1 标准 TLB，3 fixed mapping（没有 TLB）
	ext		t1, t0, 7, 3       	 	# extract MT field
	li		t2, 0x1					# load a 1 to check against
	bne		t1, t2, done_init_tlb	# 如果不等于 1，那么没有 TLB，直接结束
	mfc0	t0, C0_CONFIG1			# C0_Config1，存在 t0
	nop


start_init_tlb:
	# Config1MMUSize == Number of TLB entries - 1
	# C0_CONFIG1 寄存器的 [30:25] 表示 MMU size：值为 TLB entry 数量 - 1
	ext		t1, t0, 25, 6           # extract MMU Size, 存在 t1
	# 对于缓冲寄存器，无脑置零就对了
	mtc0    zero, C0_ENTRYLO0      	# write C0_EntryLo0
	mtc0    zero, C0_ENTRYLO1     	# write C0_EntryLo1
	mtc0    zero, C0_PAGEMASK       # write C0_PageMask
	mtc0    zero, C0_WIRED       	# write C0_Wired
	# wired 寄存器是 TLB 中 wired entry（相当于 cache locked）和 random entry 的分割
	
	lui	    t2, 0x8000				# 要给每个 entry 赋值不同的 VA，t2 作为起始地址


next_tlb_entry_pair:
	mtc0    t1, C0_INDEX            # write C0_Index 为 TLB entry 数量-1，从后往前开始赋值
	mtc0	t2, C0_ENTRYHI          # write C0_EntryHi
	ehb
	# 重申 ehb 指令：exception hazard barrier，用来插入足够多的 nop，规避流水冒险
	 
	tlbwi							# 写 TLB entry
	add	    t2, t2, (1<<14)         # Add 8K to the address to avoid TLB conflict with previous entry

	bne		t1, zero, next_tlb_entry_pair
	add	    t1, t1, -1
	

done_init_tlb:
    jr      ra
    nop
END(init_tlb)