#include <env.h>
#include <pmap.h>
#include <printf.h>

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
int remaining_time;

// 每一次进行时钟中断时，都会跳转到该函数, 进行进程的调度
// 放弃当前进程的CPU时间
void sched_yield()
{

	printf("\n\n### sched_yield -->CP0_status: 0x%x\n", get_status());

	struct Env *e = curenv;
	if(curenv == NULL)              // 第一次进时间中断
	{
		e = env_runnable_head; 
		printf("****************** first sched ******************* \n");
	}
	else							// 根据优先级进行调度
	{
		int highestPt = 0;
		struct Env *tempE = env_runnable_head;
		// 遍历一次，同时维护最高优先级和优先级最高的进程
		do { 
			if(tempE->env_pri > highestPt) {
				highestPt = tempE->env_pri; 
				//e = tempE; 
			}
			tempE = tempE->env_link;
			

		} while(tempE != env_runnable_head);
		//根据XQY的代码修改
		do{
			e=e->env_link;
		}while(e->env_pri<highestPt && e->env_status != ENV_RUNNABLE);

		printf("\ncur env_id: 0x%x\n", curenv->env_id);
		printf("next env_id: 0x%x\n", e->env_id);
		if(e == NULL)         // todo 理论上不会出现, 得放个进程在里面
		{
			printf("fail! empty sched queue!!!\n");
			while(1) ;
		}
	}

	env_run(e);
	printf("\n!!!!!!!!!!!env: 0x%x has run!!!!!!\n", e->env_id);
}


