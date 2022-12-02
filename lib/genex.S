#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <stackframe.h>

# 汇编中引用头文件的问题，# 是否还代表注释，有一些需要用到的函数并没有引入相关的头文件?

# 以上不能删
.macro	__build_clear_sti
	STI
.endm

.macro	__build_clear_cli
	CLI
.endm


NESTED(handle_tlb, TF_SIZE, sp)
	nop
	SAVE_ALL # 把32个寄存器以及cp0以及几个特殊寄存器的内容按指定顺序存到栈中
	

	li	k0,0x1800            # k0寄存器=0x1800，第11位和12位为1
    mtc0	k0, CP0_PAGEMASK # MANUALP143 ，表示页大小为4K

	mfc0	k0,CP0_BADVADDR  # 引发中断的虚拟地址
	# a0,a1是后面va2pa()的两个参数,两个参数的意义?
	mfc0	a1,	CP0_BADVADDR
	lw	a0, mCONTEXT
   		 
	                         
	jal		va2pa_print		# 这个函数会查页表，看有没有虚拟地址对应的物理页
	nop
	# V0是va2pa返回值，全1代表没有这个页，否则代表物理地址，相等表示需要新建页
	li		t0, 0xFFFFFFFF 
	beq		v0, t0, page_fault  
	nop	
	j	b1      # 不用新建页，只需要把映射关系填到tlb里
	nop

page_fault:  	# page fault occur，新建页
	
	               # 引发中断的虚拟地址
	# 传参
	mfc0	a0,CP0_BADVADDR  
	lw		a1,mCONTEXT
	             

	jal	pageout    # 创建页
	nop
	# 这里pageout()返回了物理地址在v0，直接过就行

	
b1:   # 判断是奇数还是偶数页
	move 	k1, v0  # v0 is pa
	srl		k1, 12  # 逻辑右移12
	sll 	k1, 6   # 逻辑左移6，最后6位清零，最前面6位清零
	or		k1, 0x6 # D位和V位置为1,到这里k1就可以填某一个ENTRYLO寄存器了

	
	mfc0	k0,CP0_BADVADDR  # 暂存，下面用来转成另一个地址
	andi	k0, 0x00001000    # 提取第12位，判断奇数还是偶数  是第12位还是第13位?
    bne		k0,zero,SET_ENTRYLO1  # 不为0，为奇
	nop
SET_ENTRYLO0: # 为什么要奇偶一起填?
	mtc0 	k1, CP0_ENTRYLO0  
	mtc0    zero,CP0_ENTRYLO1
	ehb
	ori		a1, 0x00001000 # 把虚拟地址12位变1，作为后面参数,获取当前偶数异常地址的相邻奇数页
	lw		a0, mCONTEXT 

	jal 	va2pa_print   # 查询相邻的奇数页是否有对应的物理地址
	nop
	li		t0, 0xFFFFFFFF  

	beq		v0, t0, tlb_refill_done # 如果没有奇数页，那么就不用管了，直接充填偶数页就行，如果有奇数页，那么就要把它存到CP0_ENTRYLO1中一起充填
	nop
	move 	k1, v0      # v0 is pa
	srl		k1, 12      # 逻辑右移12
	sll 	k1, 6       # 辑左移6，最后6位清零，最前面6位清零
	or		k1, 0x6     # D位和V位置为1
	mtc0    k1,CP0_ENTRYLO1  # 本来缺失的是偶数页的地址，但是填的时候将其相邻的页也填上去了
	j		tlb_refill_done
	nop	
SET_ENTRYLO1:
	
	mtc0    k1, CP0_ENTRYLO1
	mtc0    zero,CP0_ENTRYLO0
	ehb
	and		a1, 0xFFFFEFFF # 把12位变0
	lw		a0, mCONTEXT # a0 = 8040_0000
	jal 	va2pa_print  
	nop
	li		t0, 0xffffffff  
	beq		v0, t0, tlb_refill_done
	nop

	move 	k1, v0		   # v0 is pa
	srl		k1, 12         # 逻辑右移12
	sll 	k1, 6          # 逻辑左移6，最后6位清零，最前面6位清零
	or		k1, 0x6        # D位和V位置为1
	mtc0    k1,CP0_ENTRYLO0	

tlb_refill_done:
	# todo TLB重填
	tlbp
	# nop
	# nop
	# nop
	# nop
	mfc0 k0,CP0_INDEX
	// if co0_index==1 goto oldTlb
	# li	k1,0x1
	# beq	k0,k1,oldTlb
	bgtz 	k0, oldTlb
	nop
	nop
	jal getNextTlb

	nop
	mtc0    v0, CP0_INDEX 
	# tlbwi


oldTlb:	
	ehb # 确保前面所有操作已完成
	tlbwi
	nop
	j	ret_from_exception
	nop
	
END(handle_tlb)


NESTED(handle_mod, TF_SIZE, sp)
	nop
	# CLI
	.set at # 开启at寄存器警告
	move	a0, sp
	                 # 把函数入口传给v0
	
	nop
	j	ret_from_exception
	nop
END(handle_mod)

NESTED(handle_reserved, TF_SIZE, sp)   # 从中断中恢复中断返回用户态
	nop           # 空转指令确保安全，类似使得cpu指令按字对齐后取下一个未知的，使得CPU的PIPELINE不会错误的执行一个预取得指令
	# CLI         # 关中断
	.set at       # 允许汇编器使用$at寄存器

	nop
	j	simple_return
	nop
END(handle_reserved)

FEXPORT(ret_from_exception) # 异常恢复
	.set noreorder #禁止编译器优化指令顺序

	RESTORE_ALL
	nop
	# 判断是否嵌套
	# 判断引起该异常的EPC是否位于内核区域，恢复的EPC应该就是引起正在处理的该异常的引发地址(其他寄存器保存的是)
	mfc0	k1,CP0_EPC,2 # k1存储EPC，即处理结束后的返回地址
	li	k0,-0x80000000
	addu k0,k1
	bgez k0,core_ret # 如果是嵌套，跳转至 core_ret 进行处理

	nop
nor_ret:  # 非嵌套异常，使用 ERET 将 EXL 清零，返回用户态     
	# STI
	eret
	j cont
	nop
core_ret: # 嵌套异常
	# 恢复上一个异常的EPC 是否还需要恢复其他CP0寄存器？上一个异常恢复之前也会 RESTORE_ALL，（注意是先RESTORE后读EPC）因此这里不需要单独操作
	# lw	k0,TF_EPC(sp)
	# mtc0	k0,CP0_EPC 
cont: # continue
	j k1 # 跳转回发生异常的地址继续执行
	nop
	# 若是用户态 --> 内核态，则 k0 = sp, sp = *kernelsp - PT_SIZE，store k0, PT_R29(sp)，保存其它寄存器
	# 若是内核态 --> 内核态，直接 k0 = sp, sp = sp - PT_SIZE，store k0, PT_R29(sp)，然后保存其它寄存器
								 
FEXPORT(simple_return)#没有restoreall
	.set noreorder

	nop
	# 判断是否嵌套
	mfc0	t1,CP0_EPC,2   
	li	t0,-0x80000000
	addu t0,t1
	ehb
	bgez t0,core_ret_1     #epc >= 0x80000000，嵌套
	nop            
	# STI
	eret
	nop
core_ret_1:
	j t1
	nop


NESTED(handle_addr, TF_SIZE, sp)
	nop
	# #######################################
	mfc0 k1, C0_BADVADDR # 读出异常产生地址(试图访问的越界地址)
	li	k0,0x90000000
	beq k0,k1,kill_progress # if(bad_addr == 0x90000000: kill progress)

	# 理论上该异常一定是非嵌套异常

	# 返回 pc+4 继续执行
	mfc0	k1,CP0_EPC,2 # 读出pc
	addi k1, k1, 4 # pc = pc+4
	nop
	eret # 返回用户模式
	nop
	j k1 # 跳回k1继续执行

	# #######################################
kill_progress:
	nop
	li		sp,0x80400000#切到内核SP  
	# CLI
	.set at # 开启at寄存器警告
	mfc0 t0, CP0_CAUSE     # 取出上一次exception的cause
	mfc0 t2, CP0_STATUS    # 取出Processor status
	and t0, t2 # IM位和IP位与运算

	andi t1, t0, STATUSF_IP0  # t0和立即数0X400相与，取出t0第10位(IP1&IM1)，结果存t1，
	jal print_addr_error    # 进程模块中实现的函数
	j simple_return # 不需要恢复上下文
	
	nop
END(handle_addr)

NESTED(handle_mult_tlb, TF_SIZE, sp)  # machie check异常，多个tlb项相同，理论上不会触发
	nop
	# CLI
	.set at # 开启at寄存器警告
	nop

	mfc0 t1,CP0_STATUS
	nop
	                       # 清掉tlb多个重复的异常
	and		t1,t0
	mtc0	t1,CP0_STATUS
	ehb
	j	simple_return
	nop
	nop
END(handle_mult_tlb)


.set noreorder
# .align	5
NESTED(handle_int, TF_SIZE, sp)
.set at
nop

SAVE_TF
.set	noat # 关闭关于at寄存器的警告

# CLI
mfc0	t0, CP0_CAUSE  # 取出上一次exception的cause
mfc0	t2, CP0_STATUS # 取出Processor status

and	t0, t2  

andi	t1, t0, STATUSF_IP0   # t0和立即数0X400相与，取出t0第10位，结果存t1，

j timer_irq

nop
END(handle_int)

	.extern delay

button_deal:
	# 处理键盘



	nop


timer_irq: # 用来清空计时器并调用 sched_yield 函数
	nop
	# 处理定时器
#	j	clear_timer0_int
#	j	sched_yield # sched_yield 中会从PCB中恢复上下文和EPC
#	j	ret_from_exception # simple_return?

	# sched_yield 中并没有跳转，这里是否需要跳转，跳转到EPC?（EPC其实就是当前上下文中PC的值）
	# 是否要重置计时器?
	jal		clear_timer0_int	# clear timer0
	nop
	
	jal		sched_yield			# change process
	nop

	j 		ret_from_exception
	# mfc0	t1,CP0_EPC,2
	# j t1
	nop




