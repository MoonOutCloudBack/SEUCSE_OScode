/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
.set nomips16
#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <m32c0.h>
.section .text.exc_vec3
# NESTED(except_vec3, 0, sp)
# 		.set	noat
# 		.set	push
# 		#.set	noreorder

__tlb_refill:
		nop
		j _mips_general_exception
		nop
		.org 0x80
__xtlb_refill_loop:
		nop
		j _mips_general_exception
		nop
		.org 0x100
__cache_error_loop:
		nop
		j _mips_general_exception
		nop

		.org 0x180
_mips_general_exception:  # 中断处理地址
		/*
		 * Register saving is delayed as long as we dont know
		 * which registers really need to be saved.
		 */
#   1:	//j	1b
# 	nop

		sw		sp, kernel_sp 
		mfc0	k1,CP0_CAUSE
		
		la	k0, exception_handlers # la命令应该是加载内存地址


		/*
		 * Next lines assumes that the used CPU type has max.
		 * 32 different types of exceptions. We might use this
		 * to implement software exceptions in the future.
		 */

		andi k1,0x7c # 取出中断号 0x7c = 01111100,对应CAUSE的 ExcCode 位，因此k1存储的实际上是左移两位的中断号
		addu k0,k1 # k0 = k0+k1，即为中断向量表中的相应处理地址 (由于一个地址占4个存储单元，而k1正好是中断号*4，因此直接相加即可)
		lw k0,(k0) # 把内存中k0地址处的值赋给k0

		nop
		jr k0 #跳到对应中断处理地址(上下文的保存应该是由各自的处理程序完成的，因为不同的异常对上下文保存的要求不同)
		
		nop
		# END(except_vec3)
		# .set	at
		# .set	pop


.data # 数据段开始标志
			.globl kernel_sp		# save the sp register when exception occur
kernel_sp:
			.word 0

			.globl mCONTEXT
mCONTEXT:	# 应该就是放着内核栈底常数 0x80400000
			.word 0

			.globl exception_handlers
exception_handlers: # 应该就是中断向量的入口地址，从这里开始按中断号索引可以找到每个中断的处理地址
			.word 0:31 # .word就是指在这里放一个常量,exception_handlers是其地址（31表示连续放31个0）

			.globl delay
delay:
			.word 0

			.globl temp
temp:
			.word 16

            .globl curtf
curtf:
			.word 0
			
			.section .data.stk
KERNEL_STACK:
			.space 0x8000





.text # 代码段开始标志
LEAF(_start)

	.set	reorder
	.set	at
	/* Disable interrupts */
	mtc0	zero, CP0_STATUS

        /* Disable watch exception. */
        mtc0    zero, CP0_WATCHLO
        mtc0    zero, CP0_WATCHHI

	
	/* Zero the BSS */
	# test code 
	li v0, 0x80000000
	li v1, 0x08000060
	sw v1, 0(v0) # 将 0x08000060 写入 0x08000000上
	addi v0, 4 # v0改为下一条指令的地址

	li v1, 0x00000000
	sw v1, 0(v0) # 将 0x00000000 写入 0x08000004上
	addi v0, 4 # 再把v0改为下一条指令的地址

	
	


    la v0, _fbss
	la v1, _end
	beq v0,v1,2f # if([v0]==[v1]) PC = PC + signExtened(2f<<2)
1:
	addiu v0,v0,4
	nop
	sw zero,-4(v0)
	nop
	bne v0,v1,1b
2:

	/* disable kernel mode cache */
	# 重点是修改 Config Register 的 2:0 位
	mfc0    t0, CP0_CONFIG # 读取CP0_CONFIG寄存器
	and     t0, ~0x7 # ~0x7 应该是对 0000_0111 做1扩展，即ffff_fff7，把config的后三位置0
	ori     t0, 0x2 # 或立即数，[t0] = [t0] OR 0000_0010 ，把倒数第2位置1
	mtc0    t0, CP0_CONFIG

	/* set up stack */
	# KERNEL的栈顶位置,16K
	li      sp, 0x80400000 # 切到内核栈顶

	li      t0,0x80400000
	sw      t0,mCONTEXT

	# Clear boot interrupt vector bit in Coprocessor 0 Status register
	# macro: mips32_bicsr (SR_BEV);


	/* jump to main */

	# UM=0;CP0=0;IE=0,内核态，开中断 
	# 这里和讲义上写的不一样?
	# !! 负责运行bootloader的main() 和init里的main不是同一个东西
	mfc0 t0, CP0_STATUS
	ori t0, 0x1 # 最后一位置1，IE=1
	mtc0 t0, CP0_STATUS #结果写入status寄存器

	jal     main

	nop


loop:
	j	loop
	nop
END(_start)

