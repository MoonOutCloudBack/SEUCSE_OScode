/* See COPYRIGHT for copyright information. */


#ifndef _ENV_H_
#define _ENV_H_

#include "types.h"
#include "queue.h"
#include "trap.h"
#include <mmu.h> 

#define LOG2NENV	10
#define NENV		(1<<LOG2NENV)
#define ENVX(envid)	((envid) & (NENV - 1))
// #define GET_ENV_ASID(envid) (((envid)>> 11)<<6)
#define GET_ENV_ASID(envid) (envid & 0xFF)

// Values of env_status in struct Env
#define ENV_FREE			0
#define ENV_RUNNABLE		1
#define ENV_NOT_RUNNABLE	2
#define ENV_SUSPEND			3
#define dying				4

struct Env {
	struct Trapframe env_tf;        // Saved registers，用来存储进程的上下文，
									// 包括 32 个通用寄存器，cp0_cause 寄存器，cp0_epc 寄存器等
									// 由于每个进程我们分配独立的用户栈，因此在用户栈中我们取一段连续的空间(&env_tf), 
									// 用来存储当前进程的上下文。
									
	struct Env* env_link;       	// Free list
	u_int env_id;                   // Unique environment identifier
	u_int env_parent_id;            // env_id of this env's parent
	u_int env_status;               // Status of the environment
	Pde  *env_pgdir;                // Kernel virtual address of page dir, 存储当前进程的页表的虚拟地址
	u_int env_cr3;
	// LIST_ENTRY(Env) env_sched_link;
    u_int env_pri;
	// Lab 4 IPC
	u_int env_ipc_value;            // data value sent to us 
	u_int env_ipc_from;             // envid of the sender  
	u_int env_ipc_recving;          // env is blocked receiving
	u_int env_ipc_dstva;			// va at which to map received page
	u_int env_ipc_perm;				// perm of page mapping received

	// Lab 4 fault handling
	u_int env_pgfault_handler;      // page fault state
	u_int env_xstacktop;            // top of exception stack

	// Lab 6 scheduler counts
	u_int env_runs;					// number of times been env_run'ed，该进程已经跑过的次数
	u_int env_nop;                  // align to avoid mul instruction
	u_int heap_pc;
	uint32_t va;
};
struct EnvNode{
	struct Env *data;
	struct Env *next;

};
// LIST_HEAD(Env_list, Env);
extern struct Env *envs;			// All environments, 包含我们所有的进程控制块, 定义在 env.c 里
extern struct Env *curenv;	        // the current env
// extern struct Env_list env_sched_list[2]; // runnable env list

extern struct Env *env_runnable_head;
extern struct Env *env_runnable_tail;
extern struct Env *env_free_list; 

void env_init(void);
int env_alloc(struct Env **e, u_int parent_id);
int env_free(struct Env *);
void env_create_priority(char *binary, int priority);
void env_create(char *binary, int* pt);

int envid2env(u_int envid, struct Env **penv, int checkperm);
void env_run(struct Env *e);
#endif


