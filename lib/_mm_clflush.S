#define _BOOTCODE 1

#include "boot.h"
#include <mips/regdef.h>
#include <mips/asm.h>
#include <mips/m32c0.h>

	.set	noreorder           # Don't allow the assembler to reorder instructions.
	.set	noat                # Don't allow the assembler to use r1(at) for synthetic instr.


# 对给出的内存地址 a0，hit writeback + hit invalid
LEAF(_mm_clflush)
    # cache 里的地址应该是 VA 吧，a0 给 VA 应该没问题

    cache   0x19, 0(a0)     # op: 110 01, hit writeback, dcache
    nop
    nop
    nop
    nop
    cache   0x11, 0(a0)     # op: 100 01, hit invalid, dcache
    nop
    nop
    nop
    nop
    jr      ra              # 重申：ra（通用寄存器 31）代表当前 subroutine 的 return addr，相当于 C++ return
    nop
    nop
    nop
    nop
END(_mm_clflush)


# 在 manual 324 / 365 页
# cache 指令：cache op, offset(base)
	# 指令格式：(31)101111(26) (25)base-5(21) (20)op-5(16) (15)offset-16(0)
	# offset(base_reg)：
		# 将 16 位 offset 符号扩展，并与 base_reg 的值相加，得到参数
	# op 的 [17:16] 位指定了要操作的 cache：
		# 00 I 第一级 icache
		# 01 D 第一级 dcache 或整个的第一级 cache（如果没分 icache dcache）
		# 10 T 第三级。 11 S 第二级。（不过咱应该只有一级 cache 吧）
	# op 的 [20:18] 位为操作类型：
		# 后面 offset(base) 参数为 cache index：
			# 000：设为 invalid
			# 001：load tag，把 tag 读到 CP0_TAGLO 里
			# 010：store tag，从 CP0_TAGLO 写入 tag
			# 011：store data，从 CP0_DATALO 里写入 cache 的 data 段
		# 后面 offset(base) 参数为内存 addr：
			# 100：hit invalid，如果 cache 里有给出的 addr，那么设它为 invalid
			# 101：对 icache 来说，用该 addr 的内容 fill 它；对 dcache 来说，好像还是 hit invalid？
			# 110：Hit Writeback，如果这个 line 是 dirty 的，强制写回（并且仍保持在 valid)
			# 111: fetch and lock，用该 addr 的内容 fill 它，并且把 cache 锁住（即，我们认为这个数据很重要，就不要把它换出来了）