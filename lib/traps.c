#include <trap.h>
#include <env.h>
#include <printf.h>

// 这里导入了 genex.S 中定义的各异常处理函数
extern void handle_int();
extern void handle_reserved();
extern void handle_tlb();
extern void handle_sys();
extern void handle_mod();
extern void handle_addr();
extern void handle_mult_tlb();

extern unsigned long exception_handlers[32];  //exception_handlers define at start.S

void trap_init()
{
    int i;

    for (i = 0; i < 32; i++) {
        set_except_vector(i, handle_reserved);
    }

    // 虽然中断向量表设置了32大小，但是这里只用到了其中几个异常处理
    set_except_vector(0, handle_int); 
    set_except_vector(1, handle_mod);
    set_except_vector(2, handle_tlb); 
    set_except_vector(3, handle_tlb);
    set_except_vector(4, handle_addr);
    set_except_vector(5, handle_addr);
    set_except_vector(8, handle_sys);
    set_except_vector(24, handle_mult_tlb);
}
void *set_except_vector(int n, void *addr)
{
    unsigned long handler = (unsigned long)addr;
    unsigned long old_handler = exception_handlers[n];
    exception_handlers[n] = handler;
    return (void *)old_handler;
}


struct pgfault_trap_frame {
    u_int fault_va;
    u_int err;
    u_int sp;
    u_int eflags;
    u_int pc;
    u_int empty1;
    u_int empty2;
    u_int empty3;
    u_int empty4;
    u_int empty5;
};


void
page_fault_handler(struct Trapframe *tf)
{
    struct Trapframe PgTrapFrame;
    extern struct Env *curenv;

    bcopy(tf, &PgTrapFrame, sizeof(struct Trapframe));

    if (tf->regs[29] >= (curenv->env_xstacktop - BY2PG) &&
        tf->regs[29] <= (curenv->env_xstacktop - 1)) {
            tf->regs[29] = tf->regs[29] - sizeof(struct  Trapframe);
            bcopy(&PgTrapFrame, (void *)tf->regs[29], sizeof(struct Trapframe));
        } else {
            tf->regs[29] = curenv->env_xstacktop - sizeof(struct  Trapframe);
            bcopy(&PgTrapFrame,(void *)curenv->env_xstacktop - sizeof(struct  Trapframe),sizeof(struct Trapframe));
        }
    //  Set EPC to a proper value in the trapframe
	tf->cp0_epc=curenv->env_pgfault_handler;
    return;
}
