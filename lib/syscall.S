#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>
#include <stackframe.h>
#include <unistd.h>


NESTED(handle_sys,TF_SIZE, sp)
    # save cpu s registers
    SAVE_ALL
    # close interrput
    # CLI
    nop
    .set at

    # 系统调用号为什么是从v0拿的? 讲义中写的是从a0中拿
    # 系统调用的参数是否需要处理?
    # 异常处理为什么一会用k寄存器一会用t寄存器?
    addiu   a0, a0, -__SYSCALL_BASE  # a0 = a0 - __SYSCALL_BASE，得到相对偏移
    # _if(a0 < __NR_SYSCALLS+1) t0=1 else t0=0  __NR_SYSCALLS是系统调用总个数
    li  t2, -__NR_SYSCALLS
    addu t2, a0 # [t2] = [v0] - __NR_SYSCALLS，如果 [t2]<=0，t0=1，否则t0=0
    # li  t1, -0x1
    # addu t2, t1 # [t2] = [v0] - __NR_SYSCALLS - 1
    blez t2,core_ret_1 # if [t2]<=0,goto set_t0_1
    li  t0,0x0
    j end_set_t0
set_t0_1:
    li  t0,0x1

end_set_t0:

    #  set epc as epc plus 4 对于系统调用异常来说，处理返回地址是下一条指令的地址，而非当前指令的地址
    #  Where is the next being exectued cpu instruction when return from exception?
    lw t1, TF_EPC(sp)
    addi t1, t1, 4

    sw t1, TF_EPC(sp)   # 系统调用结束后得进行下一条指令

    #  ——if legal , goto the right function s address
    beq     t0, zero, illegal_syscall # _if t0 = 0, goto illegal_syscall
    nop
    sll     t0, v0, 2            # 把序号偏移左移两位，相当于乘以4，得到一个地址偏移（一个地址4字节
    la      t1, sys_call_table   # load address : t1 <- sys_call_table 总入口
    addu    t1, t1, t0           # t1 = t1 + t0 得到具体入口
    lw      t2, 0(t1)            # t2 = MEM(t1)  取这个位置的指针
    beq     t2, zero, illegal_syscall # _if t2 = 0, goto illegal_syscall
    nop


    nop
    la  ra,handle_return # 把handle_return作为系统调用的返回地址
    # go to exectue the syscall
    jalr    t2      # t2 is syscall function's address
    nop

handle_return:    

    # 返回值存在 v0寄存器中，将其保存到相应的内核栈中
    sw  v0, TF_REG2(sp) #   把返回值放进去

    j   ret_from_exception
    nop

    # _error and loop
illegal_syscall:

    move a0,t0        # t0为中断号*4
    jal   print_illegal
    j   ret_from_exception
    nop

END(handle_sys)

    .extern sys_putchar
    .extern sys_getenvid
    .extern sys_get_shm
    .extern sys_env_create
    .extern sys_set_pgfault_handler
    .extern sys_mem_alloc
    .extern sys_mem_map
    .extern sys_mem_unmap
    .extern sys_pthread_create
    .extern sys_set_env_status
    .extern sys_set_trapframe
    .extern sys_panic
    .extern sys_ipc_can_send
    .extern sys_ipc_recv
    .extern sys_free_myself
    .extern sys_write_dev
    .extern sys_read_dev
    .extern sys_printf
    .extern sys_set_leds
    .extern sys_get_switchs
    .extern sys_readline
    .extern sys_env_create_1
    .extern sys_mkdir
    .extern sys_cd
    .extern sys_fcraete
    .extern sys_fread
    .extern sys_fwrite
    .extern sys_ls
    .extern sys_rm
    .extern sys_rt_write_byte
    .extern sys_rt_require_device
    .extern sys_rt_release_device
    .extern sys_rt_claim_device
    .extern sys_rt_write_by_num
    # //Overview:
    # //syscalltable stores all the syscall function s entrypoints

.macro syscalltable
    .word sys_putchar
    .word sys_getenvid
    .word sys_get_shm
    .word sys_env_create
    .word sys_set_pgfault_handler
    .word sys_mem_alloc
    .word sys_mem_map
    .word sys_mem_unmap
    .word sys_pthread_create
    .word sys_set_env_status
    .word sys_set_trapframe
    .word sys_panic
    .word sys_ipc_can_send
    .word sys_ipc_recv
    .word sys_free_myself
    .word sys_write_dev
    .word sys_read_dev
    .word sys_printf
    .word sys_set_leds
    .word sys_get_switchs
    .word sys_readline
    .word sys_env_create_1
    .word sys_mkdir
    .word sys_cd
    .word sys_fcraete
    .word sys_fread
    .word sys_fwrite
    .word sys_ls
    .word sys_rm
    .word sys_rt_write_byte
    .word sys_rt_require_device
    .word sys_rt_release_device
    .word sys_rt_claim_device
    .word sys_rt_write_by_num
    .word sys_rt_exit
.endm
EXPORT(sys_call_table)

    syscalltable
.size sys_call_table, . - sys_call_table
