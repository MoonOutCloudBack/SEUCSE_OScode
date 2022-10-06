#include <env.h>
#include <pmap.h>
#include <printf.h>

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

void sched_yield()
{

	printf("\n\n### sched_yield -->CP0_status: 0x%x\n",get_status());


	struct Env *e=curenv;
	if(                  )              //第一次进时间中断
	{
		

		printf("****************** first sched ******************* \n");
	}
	else
	{
		
		int highestPt=0;
		struct Env *tempE=env_runnable_head;
		do{
			




		}while(tempE!=env_runnable_head);
		do{
			


		}while(e->env_pri<highestPt);


		printf("\ncurenv_id: 0x%x\n",curenv->env_id);
		printf("nextenv_id: 0x%x\n",e->env_id);
		if(e==NULL)         //todo 理论上不会出现,得放个进程在里面
		{
			printf("fail！empty sched queue!!!\n");
			while(1);			
		}
		
	}

	env_run(e);
	printf("\n!!!!!!!!!!!env: 0x%x has run!!!!!!\n",e->env_id);
}


