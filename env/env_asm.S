#include <mips/regdef.h>
#include <mips/m32c0.h>
#include <mips/asm.h>
#include <inc/trap.h>
			.data
			.globl	KERNEL_SP 
KERNEL_SP:
			.word		0



	.text
	.set noat
	
# 从栈中恢复上下文数据，写回各个寄存器
LEAF(env_pop_tf)
	.set noreorder

	# 有些版本会改 sp 和 ra（stack pointer, return addr）
	# sp：先 addi sp,sp,-32   在最末尾又 addi sp,sp,32
	# ra：先 sw ra,28(sp)     在最末尾又 lw ra,28(sp)       然后 jr ra
	# 虽然我并不懂

	# ## RE: 是否需要那样写与caller还是callee清理堆栈有关，可以参考函数调用规范(Calling convention) 。
	# C语言默认的cdecl是caller清理堆栈。不过这里只有一个参数，直接寄存器传参就行了


	# 首先得到 要恢复进程的上下文地址 &env_tf 的基址
	move	k0, a0			# k0 = &(curenv->env_tf), k0 - k7 是 kernel registers

	# 从该基址加载 CP0 的状态寄存器
	# TF_CAUSE、TF_LO、TF_HI等宏保存的是相对于k0的偏移
	lw 		k1, TF_CAUSE(k0)
	mtc0	k1, C0_CAUSE
	lw		v1,	TF_LO(k0)
	mtlo	v1	# 恢复累加器低位
	lw		v0,	TF_HI(k0)
	mthi	v0	# 恢复累加器高位
	lw 		v1, TF_EPC(k0)
	# li		v1, 0x15001f90 # 这是什么魔数？
	mtc0	v1,	C0_EPC
	ehb

	# 从该基址加载各个通用寄存器
	lw		$31,TF_REG31(k0)
	lw		$30,TF_REG30(k0)
	lw		$29,TF_REG29(k0)															
	lw		$28,TF_REG28(k0)
	lw		$25,TF_REG25(k0)
	lw		$24,TF_REG24(k0)
	lw		$23,TF_REG23(k0)
	lw		$22,TF_REG22(k0)
	lw		$21,TF_REG21(k0)
	lw		$20,TF_REG20(k0)
	lw		$19,TF_REG19(k0)
	lw		$18,TF_REG18(k0)
	lw		$17,TF_REG17(k0)
	lw		$16,TF_REG16(k0)
	lw		$15,TF_REG15(k0)
	lw		$14,TF_REG14(k0)
	lw		$13,TF_REG13(k0)
	lw		$12,TF_REG12(k0)
	lw		$11,TF_REG11(k0)
	lw		$10,TF_REG10(k0)
	lw		$9,TF_REG9(k0)
	lw		$8,TF_REG8(k0)
	lw		$7,TF_REG7(k0)
	lw		$6,TF_REG6(k0)
	lw		$5,TF_REG5(k0)
	lw		$4,TF_REG4(k0)
	lw		$3,TF_REG3(k0)
	lw		$2,TF_REG2(k0)
	lw		$1,TF_REG1(k0)
	

	# 开启中断，设为用户模式
	mfc0	k0, C0_STATUS		# 读到 status
	li		k1, 0x11			# 设为 USER MODE, 并开启中断
	# TODO: 这里可以用ext和ins增加可读性
	# status 第 0 位是 IE 中断使能，1 为可以中断（IE = 1，EXL = 0，ERL = 0，才可以中断）
	# status 第 4 位是 UM 用户模式，0 表示 kernal mode，1 表示 user mode
	or		k0, k1				# or 一下，置几个 1
	li		k1, 0xEFDFFFFF
	# status 第 28 位（那个 E 置零的位）是 CU0，表示是否允许控制 CP0，允许则 CU0 = 1
	# user mode 大概是不允许的
	# status 第 21 位（那个 D 置零的位）是 TS，表示 TLB 是否被关闭，发生 TLB 严重错误时 TS = 1
	and		k0, k1				# and 一下，置几个 0
	mtc0	k0, C0_STATUS		# 把 status 写回去

	ehb
	eret						# 回 epc
								# eret 明明是回 ErrorEPC，不过代码都这样给了，应该可以跑通吧
								# RE eret的行为与ERL位有关。如果ERL为1，返回的是ErrorEPC，否则是EPC（RTFM p.156）
	nop


END(env_pop_tf)
	.set at

LEAF(lcontext)
	.extern	mCONTEXT
	sw		a0,mCONTEXT
	sw		a1,curtf
	jr	ra
	nop
END(lcontext)

LEAF(jump2entry)
	nop
	jr a0
	nop
END(jump2entry)



LEAF(set_asid)
	mfc0	k0, C0_ENTRYHI
	and		k0,0xFFFFFF00
	or		a0,k0
	mtc0 	a0, C0_ENTRYHI
	jr	ra
	nop
END(set_asid)

LEAF(set_epc)
	mtc0 	a0, C0_EPC
	jr	ra
	nop
END(set_epc)


LEAF(get_status)
	mfc0 	v0, CP0_STATUS
	jr	ra
	nop
END(get_status)


LEAF(get_badaddr)
	mfc0 	v0, C0_BADVADDR
	jr	ra
	nop
END(get_badaddr)

LEAF(get_asid)
	mfc0 	v0, C0_ENTRYHI
	# asid：Adress Space ID
	# 当前进程的 asid 放在 CP0 寄存器 EntryHI 里
	# 参考：https://www.cnblogs.com/Melles/p/10714224.html
	jr	ra
	nop
END(get_asid)

LEAF(get_epc)
	mfc0 	v0, C0_EPC
	jr	ra
	nop
END(get_epc)


LEAF(get_nested_epc)
	mfc0 	v0, C0_EPC, 2
	jr	ra
	nop
END(get_nested_epc)

# 实际上是CP0_STATUS的EXL位 置0（0表示user mode）
LEAF(set_exl)
	mfc0 	v0, CP0_STATUS
	li k0,0xFFFFFFFD
	and v0, k0
	mtc0 v0,CP0_STATUS
	ehb
	jr	ra
	nop
END(set_exl)

