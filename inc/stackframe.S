#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <trap.h>

.macro STI # 好像是启用中断使能，CU0和IE置1
	mfc0	t0,	CP0_STATUS
	li	t1, (STATUS_CU0 | 0x1) # STATUS_CU0 = 0x10000000
	or	t0, t1  # or rd, rs, rt => GPR[rd] <- GPR[rs] OR GPR[rt]
	mtc0	t0, CP0_STATUS
	
.endm


.macro CLI
	mfc0	t0, CP0_STATUS
	li	t1, (STATUS_CU0 | 0x1)
	or	t0, t1
	xor	t0, 0x1
	mtc0	t0, CP0_STATUS
.endm


.macro SAVE_TF # 处理时间片到期的保存上下文环境的函数，当在内核态发生到期时不保存上下文
	
	lw 		k0,curtf # curenv->env_tf，PCB中存储上下文的，若是第一次从内核进时间中断，即没有任何进程，curtf 会为 0，根据代码跳转，并不保存上下文
	beqz	k0,first_push  //k0=&(curenv->env_tf)
	 
	
	sw		$29,TF_REG29(k0) #即保存到k0指针所在的内存中，即直接保存到PCB中而非内核栈中，因为会直接切换进程而非再次恢复执行，因此也没有restore_tf
	sw		$2,TF_REG2(k0)
	mfc0 	k1, CP0_STATUS
	sw 		k1, TF_STATUS(k0)
	mfc0    k1, CP0_CAUSE
	sw		k1, TF_CAUSE(k0)
	mfc0	k1, CP0_ENTRYHI
	sw  	k1, TF_HI(k0)
	mfc0	k1, CP0_ENTRYLO0
	sw		k1, TF_LO(k0)
	mfc0    k1, CP0_EPC #时间中断不嵌套
	sw 		k1, TF_EPC(k0)

	sw		$31,TF_REG31(k0)
	sw		$30,TF_REG30(k0)
														
	sw		$28,TF_REG28(k0)
	// 没有k0,k1
	sw		$25,TF_REG25(k0)
	sw		$24,TF_REG24(k0)
	sw		$23,TF_REG23(k0)
	sw		$22,TF_REG22(k0)
	sw		$21,TF_REG21(k0)
	sw		$20,TF_REG20(k0)
	sw		$19,TF_REG19(k0)
	sw		$18,TF_REG18(k0)
	sw		$17,TF_REG17(k0)
	sw		$16,TF_REG16(k0)
	sw		$15,TF_REG15(k0)
	sw		$14,TF_REG14(k0)
	sw		$13,TF_REG13(k0)
	sw		$12,TF_REG12(k0)
	sw		$11,TF_REG11(k0)
	sw		$10,TF_REG10(k0)
	sw		$9,TF_REG9(k0)
	sw		$8,TF_REG8(k0)
	sw		$7,TF_REG7(k0)
	sw		$6,TF_REG6(k0)
	sw		$5,TF_REG5(k0)
	sw		$4,TF_REG4(k0)
	sw		$3,TF_REG3(k0)
	
	sw		$1,TF_REG1(k0)
	ehb
first_push:

	li		sp,0x80400000//切到内核SP  

.endm

.macro SAVE_ALL # 产生异常时将寄存器保存到内核栈中
	
	mfc0	k1,CP0_EPC,2  #nested epc 将EPC寄存器中的中断返回地址复制到k1 应该是4对齐，然后移了两位0出来 EPC应该是由硬件将PC保存过来的
	li	k0,-0x80000000 #应该就是k0 =  -0x80000000
	addu k1,k0; # k1 = k1 + k0
	bgez k1,core_save     #epc >= 0x80000000,嵌套 (bgez表示大于等于0时转移)
	nop
	move	k0,sp #原来的sp放进k0存起来  
	li		sp,0x80400000 #切到内核SP  
	j handle_finish    
core_save: #处理嵌套异常，保持sp不变
	move	k0,sp #sp放进k0存起来
handle_finish:             
	subu	sp,sp,TF_SIZE  # TF_SIZE 定义在 trap.h 中，表示对一个进程需要保存的上下文的大小，这里把栈顶上移 TF_SIZE 的大小

	sw	k0,TF_REG29(sp) # 存原来sp(即异常发生前的sp，可以理解为异常返回地址的栈顶)  sw rt, offset(base) --> GPR[rt] -> memory[GPR[base]+offset](29号就是sp寄存器)
    
	mfc0	k0,CP0_STATUS      # 存 CP0_STATUS 寄存器              
	sw	k0,TF_STATUS(sp)                 
	mfc0	k0,CP0_CAUSE   #存 CP0_CAUSE 寄存器                  
	sw	k0,TF_CAUSE(sp)                  
	mfc0	k0,CP0_EPC,2 #nested epc  存 CP0_EPC 寄存器(上一个异常返回地址，因此专门用来处理嵌套异常)           
	sw	k0,TF_EPC(sp)
	mfc0	k0, CP0_BADVADDR        
	sw	k0, TF_BADVADDR(sp)            
	mfhi	k0          # 读hi寄存器 GPR(k0) <- HI                     
	sw	k0,TF_HI(sp)                     
	mflo	k0               # 读Lo寄存器 GPR(k0) <- Lo                
	sw	k0,TF_LO(sp)                     
	sw	$0,TF_REG0(sp)
	sw	$1,TF_REG1(sp)                    
	sw	$2,TF_REG2(sp)                
	sw	$3,TF_REG3(sp)                   
	sw	$4,TF_REG4(sp)                   
	sw	$5,TF_REG5(sp)                   
	sw	$6,TF_REG6(sp)                   
	sw	$7,TF_REG7(sp)                   
	sw	$8,TF_REG8(sp)                   
	sw	$9,TF_REG9(sp)                   
	sw	$10,TF_REG10(sp)                 
	sw	$11,TF_REG11(sp)                 
	sw	$12,TF_REG12(sp)                 
	sw	$13,TF_REG13(sp)                 
	sw	$14,TF_REG14(sp)                 
	sw	$15,TF_REG15(sp)                 
	sw	$16,TF_REG16(sp)                 
	sw	$17,TF_REG17(sp)                 
	sw	$18,TF_REG18(sp)                 
	sw	$19,TF_REG19(sp)                 
	sw	$20,TF_REG20(sp)                 
	sw	$21,TF_REG21(sp)                 
	sw	$22,TF_REG22(sp)                 
	sw	$23,TF_REG23(sp)                 
	sw	$24,TF_REG24(sp)                 
	sw	$25,TF_REG25(sp)       
	# 26和27就是k0和k1，可存可不存          
	# sw	$26,TF_REG26(sp) 				 
	# sw	$27,TF_REG27(sp) 				 
	sw	$28,TF_REG28(sp)                 
	sw	$30,TF_REG30(sp)                 
	sw	$31,TF_REG31(sp)
	nop

.endm

/*
 * Note that we restore the IE flags from stack. This means
 * that a modified IE mask will be nullified.
 */
.macro RESTORE_ALL  # 将内核栈中的内容恢复到寄存器中                               
		.set at                           	


		lw	v1,TF_LO(sp)    # v1是子程序返回值寄存器                                   
		mtlo	v1     # 写Lo寄存器 LO <- GPR[v1]                         
		lw	v0,TF_HI(sp)                     
		lw	v1,TF_EPC(sp)                    
		mthi	v0     # 写hi寄存器                          
		mtc0	v1,CP0_EPC #写CP0_EPC寄存器
		lw		v0,TF_STATUS(sp) 
		li      k0,0xFFDFFFFF 
		and		v0,k0 #将 STATUS中第22位(TS位) 置0 ，关闭TLB异常
		mtc0	v0,CP0_STATUS    
		                  
		lw	$31,TF_REG31(sp)                 
		lw	$30,TF_REG30(sp)                 
		lw	$28,TF_REG28(sp)                 
		lw	$25,TF_REG25(sp)                 
		lw	$24,TF_REG24(sp)                 
		lw	$23,TF_REG23(sp)                 
		lw	$22,TF_REG22(sp)                 
		lw	$21,TF_REG21(sp)                 
		lw	$20,TF_REG20(sp)                 
		lw	$19,TF_REG19(sp)                 
		lw	$18,TF_REG18(sp)                 
		lw	$17,TF_REG17(sp)                 
		lw	$16,TF_REG16(sp)                 
		lw	$15,TF_REG15(sp)                 
		lw	$14,TF_REG14(sp)                 
		lw	$13,TF_REG13(sp)                 
		lw	$12,TF_REG12(sp)                 
		lw	$11,TF_REG11(sp)                 
		lw	$10,TF_REG10(sp)                 
		lw	$9,TF_REG9(sp)                   
		lw	$8,TF_REG8(sp)                   
		lw	$7,TF_REG7(sp)                   
		lw	$6,TF_REG6(sp)                   
		lw	$5,TF_REG5(sp)                   
		lw	$4,TF_REG4(sp)                   
		lw	$3,TF_REG3(sp)                   
		lw	$2,TF_REG2(sp)                   
		lw	$1,TF_REG1(sp)

		lw	sp,TF_REG29(sp) # 最后把栈顶指针恢复
		ehb
		nop

		                
.endm
	






