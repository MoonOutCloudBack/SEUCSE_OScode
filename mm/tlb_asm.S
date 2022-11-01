#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>

LEAF(tlb_out) # 现在是一次清两
# 1: j 1b
	nop
	li 		k0, 0x00001800  # k0寄存器=0x1800，第11位和12位为1
	mtc0	k0, CP0_PAGEMASK # MANUALP143 ，表示页大小为4K

	mfc0	k1,CP0_ENTRYHI
	mtc0	a0,CP0_ENTRYHI # a0传的是要清空的虚拟地址
	ehb
	nop
	tlbp # 尝试查找EntryHI对应的物理页，index寄存器会有对应的序号，如果没找到，CP0_INDEX第一位会为1
	nop
	nop
	nop
	nop
	mfc0	k0,CP0_INDEX 
	bltz	k0,NOFOUND # 如果index<0,即没找到物理页，跳转
	nop    # 找到对应物理页

	
	mtc0	zero,CP0_ENTRYLO0	# 把两个都清空了
	mtc0	zero,CP0_ENTRYLO1
	
	nop
	ehb
    tlbwi # 将地址映射关系填入index对应的tlb中

NOFOUND:

	mtc0	k1,CP0_ENTRYHI # 恢复CP0_ENTRYHI
	
	j	ra # 返回
	nop
END(tlb_out)
