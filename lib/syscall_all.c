#include <mmu.h>
#include <env.h>
#include <printf.h>
#include <pmap.h>
#include <sched.h>
#include <print.h>
#include <../inc/rtThread.h>
#include <../fs/ff.h>
#include <../inc/types.h>
#include <../inc/env.h>
#include <../inc/string.h>
#include <../drivers/console.h>
#include <../drivers/leds.h>
#include <../drivers/switches.h>

extern char *KERNEL_SP;
extern struct Env *curenv;

#define BUFLEN 1024
char myelf[BUFLEN] = {0};
char myargv[BUFLEN] = {0};

extern struct Page* create_share_vm(int key, size_t size);
extern void* insert_share_vm(struct Env *e, struct Page *p);
extern void env_create_priority_arg(char *binary, int priority, char *arg);
extern void pthread_create(void *func, int arg);
extern void readline(const char *prompt, char *ret, int getargv);


/* Overview:
 * 	This function is used to print a character on screen.
 * 
 * Pre-Condition:
 * 	`c` is the character you want to print.
 */
// 向串口输出字符串 c
void sys_putchar(int sysno, int c, int a2, int a3, int a4, int a5)
{
	cputchar((char)c); // 向串口输出字符串 c
	return;
}

/* Overview:
 *	This function provides the environment id of current process.
 *
 * Post-Condition:
 * 	return the current environment id
 */
// 返回 curenv->env_id
u_int sys_getenvid(void)
{
	return curenv->env_id;
}

// 共享内存
void *sys_get_shm(int sysno, int key, int size)
{
	printf("try alloc share mm\n");
	struct Page *p = NULL;
	u_long rr;
	u_long perm;
	size_t sizeMax = ROUND(size, BY2PG);
	p = create_share_vm(key, size); // 创建共享内存，详见 mm/pmap.c
	if (p == NULL)
	{
		printf("alloc shared page failed\n");
		return NULL;
	}
	printf("alloc shared page success\n");
	void *result = insert_share_vm(curenv, p); // 共享内存页加入当前虚拟地址中，详见 mm/pmap.c
	printf("insert shared page success\n");
	return result;
}

// 创建进程
int sys_env_create(int sysno, char *binary, int pt, char *arg)
{
	// 大概是清空 myelf 和 myargv
	bzero(myelf, 1024);
	bzero(myargv, 1024);
	// for (int i = 0; i < BUFLEN; i++) { }
	if (arg)
	{
		int i = 0;
		// 把 arg 读到 myargv 里
		for (int i = 0; arg[i]; i++) {
			myargv[i] = arg[i];
		}
		myargv[i] = 0;
		printf("\n");
		// 把 binary 读到 myelf 里		
		for (int i = 0; binary[i]; i++) {
			myelf[i] = binary[i];
		}
		printf("\n");
		myelf[i] = 0;
		printf("env_create_arg(%s, %d, %s)\n", myelf, pt, myargv);
		env_create_priority_arg(myelf, pt, myargv);
	}
	else
	{
		// 把 binary 读到 myelf 里		
		for (int i = 0; binary[i]; i++) {
			myelf[i] = binary[i];
		}
		printf("\n");
		printf("env_create(%s, %d)\n", myelf, pt);
		env_create_priority(myelf, pt);
	}
}

/* Overview:
 * 	Set envid's pagefault handler entry point and exception stack.
 * 
 * Pre-Condition:
 * 	xstacktop points one byte past exception stack.
 *
 * Post-Condition:
 * 	The envid's pagefault handler will be set to `func` and its
 * 	exception stack will be set to `xstacktop`.
 * 	Returns 0 on success, < 0 on error.
 */
// 根据 envid，设置该 env 的 1. pagefault handler（通过 entry point）2. exception stack（通过 stack top）
int sys_set_pgfault_handler(int sysno, u_int envid, u_int func, u_int xstacktop)
{	
	struct Env *env;
	int ret;
	ret = envid2env(envid, &env, 0); // 得到 envid 对应的 env
	if (ret < 0)
	{
		printf("sys_set_pgfault_handler:can't get env");
		return -E_INVAL;
	}
	// Your code here.
	env->env_pgfault_handler = func;
	env->env_xstacktop = xstacktop;
	return 0;
}

/* Overview:
 * 	Allocate a page of memory and map it at 'va' with permission
 * 'perm' in the address space of 'envid'.
 *
 * 	If a page is already mapped at 'va', that page is unmapped as a
 * side-effect.
 * 
 * Pre-Condition:
 * perm -- PTE_V is required,
 *         PTE_COW is not allowed(return -E_INVAL),
 *         other bits are optional.
 *
 * Post-Condition:
 * Return 0 on success, < 0 on error
 *	- va must be < UTOP
 *	- env may modify its own address space or the address space of its children
 */
// 分配一页物理内存，并将其映射至 va 虚拟地址，在 envid 进程的 address space 里，以 perm 的权限
int sys_mem_alloc(int sysno, u_int envid, u_int va, u_int perm)
{
	struct Env *env;
	struct Page *ppage;
	int ret;
	ret = 0;

	// check whether permission is legal
	if ((!(perm & PTE_V)) || (perm & PTE_COW)) {
		// 按照原注释的意思，判一下 PTE_V PTE_COW
		printf("sys_mem_alloc:permission denined\n");
		return -E_INVAL;
	}

	// check whether va is legal
	else if (va >= UTOP || va < 0) {
		// va 必须在 0 和 user stack top 之间
		printf("sys_mem_alloc:va is illegal\n)");
		return -E_INVAL;
	}

	// try to alloc a page
	ret = page_alloc(&ppage); // 从空闲链表中分配一页物理内存，见 pmap.c
	if (ret < 0) {
		printf("sys_mem_alloc:failed to alloc a page\n");
		return -E_NO_MEM;
	}

	// try to check and get the env_id;
	ret = envid2env(envid, &env, 1); // 得到 envid 对应的 env
	if (ret < 0) {
		printf("sys_mem_alloc:failed to get the target env\n");
		return -E_BAD_ENV;
	}
	// now insert
	ret = page_insert(env->env_pgdir, ppage, va, perm); 
	// 将 va 虚拟地址、和其要对应的物理页 pp 的映射关系，以 perm 的权限，加入页目录 pde
	if (ret < 0)
	{
		printf("sys_mem_alloc:page_insert failed");
		return -E_NO_MEM;
	}

	return 0;
}

/* Overview:
 * 	Map the page of memory at 'srcva' in srcid's address space
 * at 'dstva' in dstid's address space with permission 'perm'.
 * Perm has the same restrictions as in sys_mem_alloc.
 * (Probably we should add a restriction that you can't go from
 * non-writable to writable?)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Note:
 * 	Cannot access pages above UTOP.
 */
// 将 srcid 进程空间内 srcva 虚拟地址 所对应的物理页，映射到 dstid 进程空间内的 dstva 虚拟地址
int sys_mem_map(int sysno, u_int srcid, u_int srcva, u_int dstid, u_int dstva,
				u_int perm)
{
	int ret;
	u_int round_srcva, round_dstva;
	struct Env *srcenv;
	struct Env *dstenv;
	struct Page *ppage;
	Pte *ppte;

	ppage = NULL;
	ret = 0;
	round_srcva = ROUNDDOWN(srcva, BY2PG);
	round_dstva = ROUNDDOWN(dstva, BY2PG);

	// get corresponding env
	if (envid2env(srcid, &srcenv, 1) < 0) // 如果用 id 找不到 env
	{ //=============================
		printf("sys_mem_map:srcenv doesn't exist\n");
		return -E_BAD_ENV;
	}
	if (envid2env(dstid, &dstenv, 1) < 0) // 如果用 id 找不到 env
	{ //==============================
		printf("sys_mem_map:dstenv doesn't exist\n");
		return -E_BAD_ENV;
	} 

	// va < UTOP?
	if (srcva >= UTOP || dstva >= UTOP || srcva < 0 || dstva < 0)
	{	// 和上面一样，判一下虚拟地址合法性
		// va 必须在 0 和 user stack top 之间
		printf("sys_mem_map:va is invalid\n");
		return -E_NO_MEM;
	}
	// perm is valid?
	if ((!(perm & PTE_V)) || (perm & PTE_COW))
	{	// 和上面一样，判一下 perm 合法性
		printf("sys_mem_map:permission denied\n");
		return -E_NO_MEM;
	}

	// try to get the page
	ppage = page_lookup(srcenv->env_pgdir, round_srcva, &ppte); // 找到虚拟地址 va 所在的页
	if (ppage == NULL)
	{
		printf("sys_mem_map:page of srcva is invalid\n");
		return -E_NO_MEM;
	}

	// try to insert the page
	ret = page_insert(dstenv->env_pgdir, ppage, round_dstva, perm); 
	// 将 va 虚拟地址、和其要对应的物理页 pp 的映射关系，以 perm 的权限，加入页目录 pde
	if (ret < 0)
	{
		printf("sys_mem_map:page_insert denied\n");
		return -E_NO_MEM;
	}

	return 0;
}

/* Overview:
 * 	Unmap the page of memory at 'va' in the address space of 'envid'
 * (if no page is mapped, the function silently succeeds)
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Cannot unmap pages above UTOP.
 */
// 将 envid 进程空间 va 虚拟地址对应的物理页 unmapp 
int sys_mem_unmap(int sysno, u_int envid, u_int va)
{
	int ret = 0;
	struct Env *env;
	ret = envid2env(envid, &env, 1); // 照常根据 id 找 env
	if (ret < 0) {
		printf("sys_mem_alloc:failed to get the target env\n");
		return -E_BAD_ENV;
	}
	if (va < 0 || va >= UTOP) { // 照常判 va
		printf("sys_mem_unmap:va is not valid\n");
		return -E_NO_MEM;
	}
	page_remove(env->env_pgdir, va); // 参数为 页目录、va
	return ret;
}

// 创建线程
int sys_pthread_create(int sysno, int *func, int *arg)
{
	pthread_create(func, arg);
	return 1;
}

/* Overview:
 * 	Set envid's env_status to status.
 *
 * Pre-Condition:
 * 	status should be one of `ENV_RUNNABLE`, `ENV_NOT_RUNNABLE` and
 * `ENV_FREE`. Otherwise return -E_INVAL.
 * 
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if status is not a valid status for an environment.
 * 	The status of environment will be set to `status` on success.
 */
// 将 env 的 status 设为 status
int sys_set_env_status(int sysno, u_int envid, u_int status)
{
	struct Env *env;
	int r;
	extern int cur_sched;

	// #define ENV_FREE				0
	// #define ENV_RUNNABLE			1
	// #define ENV_NOT_RUNNABLE		2
	// #define ENV_SUSPEND			3
	// #define dying				4

	// 判合法性
	if (status != ENV_RUNNABLE && status != ENV_NOT_RUNNABLE 
			&& status != ENV_FREE && status != ENV_SUSPEND && status != dying)
	{	// 判一下 status 合法性
		printf("set_env_status:wrong status");
		return -E_INVAL;
	}
	r = envid2env(envid, &env, 0); // 照常找 env
	if (r < 0) {
		printf("set_status:env is invalid\n");
		return -E_BAD_ENV;
	}

	// 根据 status 的变化，对 env 做相应的操作
	env->env_status = status;

	if (status == ENV_FREE && env->env_status != status) {

		// 1. 从 env_runnable 里删去 env
		bool Efound = true;
		struct Env *tempE = env_runnable_head;
		struct Env *tempE_pre = env_runnable_tail;
		while (tempE != env) {
			tempE_pre = tempE;
			tempE = tempE->env_link;
			if (tempE == env_runnable_head) { // 回到起点
				Efound = false; // 没找到
			}
		}
		if(Efound) { // 现在我们找到了 env，tempE 指向 env，tempE_pre 是 env 前一个
			if (tempE == env_runnable_tail){
				env_runnable_tail = tempE_pre;
			}
			if (env_runnable_head == env){
				env_runnable_head = env_runnable_head->env_link;
			}
			tempE_pre->env_link = tempE->env_link; // 从 env_runnable 里删去 env
		}

		// 2. 把 env 加入 env_free_list
		Efound = false;
		tempE = env_free_list;
		while(tempE != NULL) {
			if(tempE == env) { // env 已经在 free_list 了
				Efound = true; break;
			}
			tempE = tempE->env_link;
		}
		if (!Efound) { // 把 env 加入 env_free_list
			env->env_link = env_free_list; // env 指向 env_free_list 链表头
			env_free_list = env; // 把新的链表头赋为 env
		}
	}
	
	else if (status == ENV_RUNNABLE && env->env_status != status) {

		// 1. 把 env 加入 env_runnable
		bool Efound = true;
		struct Env *tempE = env_runnable_head;
		struct Env *tempE_pre = env_runnable_tail;
		while (tempE != env) {
			tempE = tempE->env_link;
			if (tempE == env_runnable_head) { // 回到起点
				Efound = false; // 没找到
			}
		}
		if (!Efound) { // 把 env 加入 env_runnable
			env->env_link = env_runnable_head; // env 指向 env_runnable_list 链表头
			env_runnable_head = env; // 把新的链表头赋为 env
			env_runnable_tail->env_link = env;
		}

		// 2. 从 env_free_list 里删去 env
		Efound = false;
		tempE = env_free_list;
		tempE_pre = NULL;
		while(tempE != NULL) {
			if(tempE == env) { // env 已经在 free_list 了
				Efound = true; break;
			}
			tempE_pre = tempE;
			tempE = tempE->env_link;
		}
		if (Efound) { // 从 env_free_list 里删去 env (tempE)
			tempE_pre->env_link = tempE->env_link;
		}
	}

	else if (status == ENV_NOT_RUNNABLE && env->env_status != status) {

		// 1. 从 env_runnable 里删去 env
		bool Efound = true;
		struct Env *tempE = env_runnable_head;
		struct Env *tempE_pre = env_runnable_tail;
		while (tempE != env) {
			tempE_pre = tempE;
			tempE = tempE->env_link;
			if (tempE == env_runnable_head) { // 回到起点
				Efound = false; // 没找到
			}
		}
		if(Efound) { // 现在我们找到了 env，tempE 指向 env，tempE_pre 是 env 前一个
			if (tempE == env_runnable_tail){
				env_runnable_tail = tempE_pre;
			}
			if (env_runnable_head == env){
				env_runnable_head = env_runnable_head->env_link;
			}
			tempE_pre->env_link = tempE->env_link; // 从 env_runnable 里删去e
		}

		// 2. 从 env_free_list 里删去 env
		Efound = false;
		tempE = env_free_list;
		tempE_pre = NULL;
		while(tempE != NULL) {
			if(tempE == env) { // env 已经在 free_list 了
				Efound = true; break;
			}
			tempE_pre = tempE;
			tempE = tempE->env_link;
		}
		if (Efound) { // 从 env_free_list 里删去 env (tempE)
			tempE_pre->env_link = tempE->env_link;
		}
		
	}
	
	return 0;
}

/* Overview:
 * 	Set envid's trap frame to tf.
 *
 * Pre-Condition:
 * 	`tf` should be valid.
 *
 * Post-Condition:
 * 	Returns 0 on success, < 0 on error.
 * 	Return -E_INVAL if the environment cannot be manipulated.
 *
 * Note: This hasn't be used now?
 */
// 将 env 的 tf（Saved registers，用来存储进程的上下文）设为 tf
int sys_set_trapframe(int sysno, u_int envid, struct Trapframe *tf)
{
	int ret;
	struct Env *e;
	ret = envid2env(envid, &e, 1); // 继续找 env
	if (ret < 0) {
		printf("set_trapframe:env is invalid\n");
		return -E_BAD_ENV;
	}
	e->env_tf = *tf; // 设置进程上下文, 直接指向
	return 0;
}

/* Overview:
 * 	Kernel panic with message `msg`. 
 *
 * Pre-Condition:
 * 	msg can't be NULL
 *
 * Post-Condition:
 * 	This function will make the whole system stop.
 */
// kernal panic 输出 msg
void sys_panic(int sysno, char *msg)
{
	// no page_fault_mode -- we are trying to panic!
	panic("%s", TRUP(msg));
}

// 进程间通信的系统调用：
/*
- ipc: Interprocess communication
- inc/env.h 中的定义：
	u_int env_ipc_value;		// data value sent to us 
	u_int env_ipc_from;			// envid of the sender  
	u_int env_ipc_recving;		// env is blocked receiving
	u_int env_ipc_dstva;		// va at which to map received page
	u_int env_ipc_perm;			// perm of page mapping received
- 等待收到信息的进程：
	- env_ipc_recving == 1
	- status == ENV_NOT_RUNNABLE
	- env_ipc_dstva 赋值为准备 map 发送过来的物理页的 va

*/

/* Overview:
 * 	Try to send 'value' to the target env 'envid'.
 *
 * 	The send fails with a return value of -E_IPC_NOT_RECV if the
 * target has not requested IPC with sys_ipc_recv.
 * 	Otherwise, the send succeeds, and the target's ipc fields are
 * updated as follows:
 *    env_ipc_recving is set to 0 to block future sends
 *    env_ipc_from is set to the sending envid
 *    env_ipc_value is set to the 'value' parameter
 * 	The target environment is marked runnable again.
 *
 * Post-Condition:
 * 	Return 0 on success, < 0 on error.
 *
 * Hint: the only function you need to call is envid2env.
 */
// 进程间通信：由 curenv 调用，把 srcva 对应的物理页，映射到 env 的 va（env->env_ipc_dstva） 
int sys_ipc_can_send(int sysno, u_int envid, u_int value, u_int srcva,
					 u_int perm)
{
	int r;
	struct Env *e;
	struct Page *p;
	Pte *ppte;
	extern int cur_sched;
	
	// try to get the destination env
	r = envid2env(envid, &e, 0); // 找到 target env
	if (r < 0)
	{
		printf("ipc_send:dstenv is invalid\n");
		return -E_BAD_ENV;
	}
	// check whether target env is requesting ipc
	if (e->env_status != ENV_NOT_RUNNABLE || !e->env_ipc_recving)
	{	// 判一下 target env 的状态，是否是等待收到信息
		printf("ipc_send:target env id not requesting recving\n");
		return -E_IPC_NOT_RECV;
	}
	// check whether source & target virtual address is valid
	if (srcva >= UTOP || srcva < 0)
	{	// 判一下虚拟地址 va 合法性
		printf("ipc_send:virtual address greater than UTOP\n");
		return -E_NO_MEM;
	}
	// try to get the page which will be sent later
	if (srcva != 0)
	{
		p = page_lookup(curenv->env_pgdir, srcva, &ppte);
		if (p == NULL)
		{	// 找到要收信息的那个物理页
			printf("ipc_send:destinated page not exist");
			return -E_NO_MEM;
		}
		r = page_insert(e->env_pgdir, p, e->env_ipc_dstva, perm);
		if (r < 0)
		{
			printf("ipc_send:page_insert failed\n");
			return -E_NO_MEM;
		}
	}

	// 通信完了，修改状态
	e->env_ipc_value = value;
	e->env_ipc_from = curenv->env_id;
	e->env_ipc_perm = perm;
	e->env_ipc_recving = 0;
	e->env_status = ENV_RUNNABLE;
	if(true) {
		// 1. 把 e 加入 env_runnable
		bool Efound = true;
		struct Env *tempE = env_runnable_head;
		struct Env *tempE_pre = env_runnable_tail;
		while (tempE != e) {
			tempE = tempE->env_link;
			if (tempE == env_runnable_head) { // 回到起点
				Efound = false; // 没找到
			}
		}
		if (!Efound) { // 把 e 加入 env_runnable
			e->env_link = env_runnable_head; // e 指向 env_runnable_list 链表头
			env_runnable_head = e; // 把新的链表头赋为 e
			env_runnable_tail->env_link = e;
		}

		// 2. 从 env_free_list 里删去 e
		Efound = false;
		tempE = env_free_list;
		tempE_pre = NULL;
		while(tempE != NULL) {
			if(tempE == e) { // e 已经在 free_list 了
				Efound = true; break;
			}
			tempE_pre = tempE;
			tempE = tempE->env_link;
		}
		if (Efound) { // 从 env_free_list 里删去 e (tempE)
			tempE_pre->env_link = tempE->env_link;
		}
	}
	return 0;
}

/* Overview:
 * 	This function enables caller to receive message from 
 * other process. To be more specific, it will flag 
 * the current process so that other process could send 
 * message to it.
 *
 * Pre-Condition:
 * 	`dstva` is valid (Note: NULL is also a valid value for `dstva`).
 * 
 * Post-Condition:
 * 	This syscall will set the current process's status to 
 * ENV_NOT_RUNNABLE, giving up cpu. 
 */
// 由 curenv 调用，表示自己希望接收别的进程的通信
void sys_ipc_recv(int sysno, u_int dstva)
{
	extern int remaining_time;
	if (dstva >= UTOP || dstva < 0)
	{	// 判虚拟地址是否合法
		printf("ipc_recv:dstva is greater than UTOP");
		return;
	}
	curenv->env_status = ENV_NOT_RUNNABLE;
	if(true) {
		// 1. 从 env_runnable 里删去 curenv
		bool Efound = true;
		struct Env *tempE = env_runnable_head;
		struct Env *tempE_pre = env_runnable_tail;
		while (tempE != curenv) {
			tempE_pre = tempE;
			tempE = tempE->env_link;
			if (tempE == env_runnable_head) { // 回到起点
				Efound = false; // 没找到
			}
		}
		if(Efound) { // 现在我们找到了 env，tempE 指向 env，tempE_pre 是 curenv 前一个
			if (tempE == env_runnable_tail){
				env_runnable_tail = tempE_pre;
			}
			if (env_runnable_head == curenv){
				env_runnable_head = env_runnable_head->env_link;
			}
			tempE_pre->env_link = tempE->env_link; // 从 env_runnable 里删去e
		}

		// 2. 从 env_free_list 里删去 curenv
		Efound = false;
		tempE = env_free_list;
		tempE_pre = NULL;
		while(tempE != NULL) {
			if(tempE == curenv) { // curenv 已经在 free_list 了
				Efound = true; break;
			}
			tempE_pre = tempE;
			tempE = tempE->env_link;
		}
		if (Efound) { // 从 env_free_list 里删去 curenv (tempE)
			tempE_pre->env_link = tempE->env_link;
		}
	}
	curenv->env_ipc_recving = 1;
	curenv->env_ipc_dstva = dstva;

	remaining_time = 0;
	void *src = KERNEL_SP - sizeof(struct Trapframe);
	void *dst = TIMESTACK - sizeof(struct Trapframe);
	bcopy(src, dst, sizeof(struct Trapframe)); // bit copy
	//sched_yield();
	sched_yield_voluntarily_giveup();
}

// 释放自己, 目前直接调用 env_free(), 由于 tlb_invalidate 接口问题只能接受 curenv
void sys_free_myself()
{
	env_free(curenv);
}

/* Overview:
 * 	This function is used to write data to device, which is
 * 	represented by its mapped physical address.
 *	Remember to check the validity of device address (see Hint below);
 * 
 * Pre-Condition:
 *      'va' is the startting address of source data, 'len' is the
 *      length of data (in bytes), 'dev' is the physical address of
 *      the device
 * 	
 * Post-Condition:
 *      copy data from 'va' to 'dev' with length 'len'
 *      Return 0 on success.
 *	Return -E_INVAL on address error.
 *      
 * Hint: Use ummapped segment in kernel address space to perform MMIO.
 *	 Physical device address:
 *	* ---------------------------------*
 *	|   device   | start addr | length |
 *	* -----------+------------+--------*
 *	|  console   | 0x10000000 | 0x20   |
 *	|    IDE     | 0x13000000 | 0x4200 |
 *	|    rtc     | 0x15000000 | 0x200  |
 *	* ---------------------------------*
 */
// 把起始地址 va 长度为 len 的数据，写到 dev 设备上（dev 是地址）
int sys_write_dev(int sysno, u_int va, u_int dev, u_int len)
{
	// Your code here
	u_int startaddr = dev;
	u_int endaddr = dev + len;
	if (!((dev >= 0x10000000 && endaddr <= 0x10000000 + 0x20) ||
		  (dev >= 0x13000000 && endaddr <= 0x13000000 + 0x4200) ||
		  (dev >= 0x15000000 && endaddr <= 0x15000000 + 0x200)))
	{	// 判 dev 地址合法性
		printf("sys_write_dev:invalid dev\n");
		return -E_INVAL;
	}
	startaddr += 0xa0000000; // 直接虚拟地址 -> 物理地址
	bcopy((void *) va, (void *) startaddr, len); // copy 过去
	return 0;
}

/* Overview:
 * 	This function is used to read data from device, which is
 * 	represented by its mapped physical address.
 *	Remember to check the validity of device address (same as sys_read_dev)
 * 
 * Pre-Condition:
 *      'va' is the startting address of data buffer, 'len' is the
 *      length of data (in bytes), 'dev' is the physical address of
 *      the device
 * 
 * Post-Condition:
 *      copy data from 'dev' to 'va' with length 'len'
 *      Return 0 on success, < 0 on error
 *      
 * Hint: Use ummapped segment in kernel address space to perform MMIO.
 */
// 读 dev 的设备数据，并将其写到 起始地址 va 长度为 len 的内存上
int sys_read_dev(int sysno, u_int va, u_int dev, u_int len)
{
	// Your code here
	u_int startaddr = dev;
	u_int endaddr = dev + len;
	if (!((dev >= 0x10000000 && endaddr <= 0x10000000 + 0x20) ||
		  (dev >= 0x13000000 && endaddr <= 0x13000000 + 0x4200) ||
		  (dev >= 0x15000000 && endaddr <= 0x15000000 + 0x200)))
	{	// 判 dev 地址合法性
		printf("sys_read_dev:invalid dev\n");
		return -E_INVAL;
	}
	startaddr += 0xa0000000; // 直接虚拟地址 -> 物理地址
	bcopy((void *) startaddr, (void *) va, len); // copy 回来
	return 0;
}


void sys_printf(int sysno, char *fmt, va_list *ap)
{
	lp_Print(myoutput, 0, fmt, *ap);
	va_end(*ap);
}
/**
 * This function is used to set leds by val 
 * 
 */
void sys_set_leds(int sysno, u32 val)
{
	set_leds(val);
}

/**
 * This function is used to get switchs value
 */
u32 sys_get_switchs(int sysno)
{
	return get_switches();
}

/**
 * This function is used to read a line of string entered by the user
 */
void sys_readline(int sysno, const char *prompt, char *ret, int getargv)
{
	readline(prompt, ret, getargv);
}

extern int cur_sched;
void sys_env_create_1(int sysno, void *func, int arg)
{
	pthread_create(func, arg);
}

int sys_mkdir(int sysno, char *path)
{
	if (f_mkdir(path))
	{
		printf("Failed to make directory <");
		printf(path);
		printf(">\n");
	}
	return 1;
}

int sys_cd(int sysno, char *path)
{
	if (f_chdir(path))
	{
		printf("Failed to change directory to <");
		printf(path);
		printf(">\n");
	}
	return 1;
}

int sys_fcraete(int sysno, char *fname)
{
	FIL fil;
	FRESULT fr;
	fr = f_open(&fil, fname, FA_CREATE_ALWAYS);
	if (fr)
	{
		printf("Failed to create file <");
		printf(fname);
		printf(">\n");
	}
	return 1;
}

int sys_fread(int sysno, char *path)
{
	FIL fil;
	FRESULT fr;
	fr = f_open(&fil, path, FA_READ);
	if (fr)
	{
		printf("Failed to open file!\n\r", 0);
		return (int)fr;
	}
	// uint32_t *read_file_buf = 0x89a00000;
	TCHAR *read_file_buf = 0x89a00000;
	char *str = f_gets(read_file_buf, 1024, &fil);
	str[1023] = '\0';

	if (fil.fsize != 0)
		printf(str);

	if (f_close(&fil))
		printf("Failed to close file!\n");
	printf("\n");
	return 1;
}

int sys_fwrite(int sysno, char *path, char *str)
{
	char *filename = path;
	const TCHAR *input = str;

	FIL fil;
	FRESULT fr;
	fr = f_open(&fil, path, FA_WRITE);
	if (fr)
	{
		printf("Failed to open file!\n\r", 0);
		return (int)fr;
	}
	f_puts(input, &fil);
	if (f_close(&fil))
		printf("Failed to close file!\n");
	return 1;
}

int sys_ls(int sysno)
{
	FIL fil;
	FRESULT fr;
	DIR dir;
	FILINFO fno;
	if (f_opendir(&dir, "."))
		printf("Can not open current directory!\n");
	while (1)
	{
		fr = f_readdir(&dir, &fno);
		if (fr != FR_OK || fno.fname[0] == 0)
			break;
		if (fno.fname[0] == '.')
			continue;
		if (fno.fattrib & AM_DIR)
		{
			printf("\033[1;36;40m");
			printf(fno.fname);
			printf("\033[0m");
		}
		else
			printf(fno.fname);
		printf(" ");
	}
	printf("\n");
	return 1;
}

int sys_rm(int sysno, char *path)
{
	if (f_unlink(path))
	{
		printf("Failed to remove <");
		printf(path);
		printf(">\n");
	}
	return 1;
}

bool sys_rt_write_byte(int sysno, u32 device_id, char *buf, u32 i)
{
	rt_device_write_byte(device_id, buf, i);
}
bool sys_rt_require_device(int sysno, u32 device_id, u32 num)
{
	rt_require_device(device_id, num);
}
bool sys_rt_release_device(int sysno, u32 device_id, u32 num)
{
	rt_release_device(device_id, num);
}
bool sys_rt_claim_device(int sysno, u32 *req)
{
	rt_claim_device(req);
}
bool sys_rt_write_by_num(int sysno, u32 device_id, u32 num, char *buf)
{
	rt_device_write_by_num(device_id, num, buf);
}

bool sys_rt_exit(int sysno)
{
	rt_task_exit();
}
