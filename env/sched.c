#include <env.h>
#include <pmap.h>
#include <printf.h>

#define MAX_ENV_PRIORITY 5
#define TIME_TO_MAKE_ENV_ALL_PRIORIST 5

extern u32 get_status();

/* Overview:
 *  Implement simple round-robin scheduling.
 *  Search through 'envs' for a runnable environment ,
 *  in circular fashion statrting after the previously running env,
 *  and switch to the first such environment found.
 *
 * Hints:
 *  The variable which is for counting should be defined as 'static'.
 */
extern struct Env* env_runnable_head;
extern struct Env* env_runnable_tail;
extern struct Env* env_free_list;
extern int cur_sched;
extern int remaining_time = TIME_TO_MAKE_ENV_ALL_PRIORIST;

/*

MLFQ 规则：
- 如果 A 的优先级 > B 的优先级，运行 A（不运行 B）
- 如果 A 的优先级 = B 的优先级，轮转运行 A 和 B
- 工作进入系统时，放在最高优先级（最上层队列）
  - 第一类：一旦工作用完了其在某一层中的时间配额（无论中间主动放弃了多少次 CPU），就降低其优先级（移入低一级队列）
  - 第二类：工作用完整个时间片后，降低其优先级（移入低一级队列）
- 经过一段时间 S，就将系统中所有工作重新加入最高优先级队列

仅有 syscall_all.c 的 sys_ipc_recv 函数（表示 curenv 想接收别的进程的通信，因此主动放弃时间片）是主动调用 sched_yield 的；
（也就是说，curenv 调用 sys_ipc_recv 放弃时间片时，时间片还没用完；）
所以，非常好区分 是否用完整个时间片。
所以简单起见，我们采用第二类 MLFQ 规则。
（其实计时也是可以的，可以去读 timer 的 counter，不过这样 env 的数据结构需要去改）

为实现 MLFQ，我们需要 
1. 在 env 数据结构里维护优先级；
2. 维护 离上次把所有进程捞到最高优先级 的时间（就用那个 remaining_time 了），时间到了就再捞一次。

*/


// 每一次进行时钟中断时，都会跳转到该函数, 进行进程的调度
void sched_yield() {

	printf("\n\n### sched_yield -->CP0_status: 0x%x\n", get_status());

	struct Env *e = curenv;
	struct Env *tempE = NULL;

	remaining_time -= 1; // 直接拿时间中断来粗略计时
	if(remaining_time == 0) { // 时间到了，把所有进程都捞到最高优先级
		tempE = env_runnable_head;
		do { 
			tempE->env_pri = MAX_ENV_PRIORITY;
			tempE = tempE->env_link;
		} while(tempE != env_runnable_head);
		remaining_time = TIME_TO_MAKE_ENV_ALL_PRIORIST;
	}

	if(curenv == NULL) {             // 第一次进时间中断
		e = env_runnable_head; 
		printf("****************** first sched ******************* \n");
	}
	else {							// 根据优先级进行调度
		int highestPt = 0;
		tempE = env_runnable_head;
		// 遍历一次，同时维护最高优先级和优先级最高的进程
		do { 
			if(tempE->env_pri > highestPt) {
				highestPt = tempE->env_pri; 
				e = tempE;
			}
			tempE = tempE->env_link;

		} while(tempE != env_runnable_head);

		printf("\ncur env_id: 0x%x\n", curenv->env_id);
		printf("next env_id: 0x%x\n", e->env_id);
		if(e == NULL) {         // todo 理论上不会出现, 得放个进程在里面
			printf("fail! empty sched queue!!!\n");
			while(1) ;
		}
		// curenv 优先级降一级
		if(curenv->env_pri > 0) curenv->env_pri -= 1;
	}


	env_run(e);
	printf("\n!!!!!!!!!!!env: 0x%x has run!!!!!!\n", e->env_id);
}

void sched_yield_voluntarily_giveup() {

	printf("\n\n### sched_yield_voluntarily_giveup -->CP0_status: 0x%x\n", get_status());

	struct Env *e = curenv;
	if(curenv == NULL) {             // 第一次进时间中断
		e = env_runnable_head; 
		printf("****************** first sched ******************* \n");
	}
	else {							// 根据优先级进行调度
		int highestPt = 0;
		struct Env *tempE = env_runnable_head;
		// 遍历一次，同时维护最高优先级和优先级最高的进程
		do { 
			if(tempE->env_pri > highestPt) {
				highestPt = tempE->env_pri; 
				e = tempE;
			}
			tempE = tempE->env_link;
		} while(tempE != env_runnable_head);

		printf("\ncur env_id: 0x%x\n", curenv->env_id);
		printf("next env_id: 0x%x\n", e->env_id);
		if(e == NULL) {         // todo 理论上不会出现, 得放个进程在里面
			printf("fail! empty sched queue!!!\n");
			while(1) ;
		}
	}

	env_run(e);
	printf("\n!!!!!!!!!!!env: 0x%x has run!!!!!!\n", e->env_id);
}
