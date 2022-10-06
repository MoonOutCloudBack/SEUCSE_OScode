#include <mmu.h>
#include <env.h>
#include <printf.h>
#include <pmap.h>
#include <sched.h>
#include <print.h>
#include <../inc/rtThread.h>
#include <../fs/ff.h>
extern char *KERNEL_SP;
extern struct Env *curenv;
#define BUFLEN 1024
char myelf[BUFLEN] = {0};
char myargv[BUFLEN] = {0};
/* Overview:
 * 	This function is used to print a character on screen.
 * 
 * Pre-Condition:
 * 	`c` is the character you want to print.
 */
void sys_putchar(int sysno, int c, int a2, int a3, int a4, int a5)
{
	cputchar((char)c);
	return;
}

/* Overview:
 *	This function provides the environment id of current process.
 *
 * Post-Condition:
 * 	return the current environment id
 */
u_int sys_getenvid(void)
{
	return curenv->env_id;
}

//共享内存
void *sys_get_shm(int sysno, int key, int size)
{
	printf("try alloc share mm\n");
	struct Page *p = NULL;
	u_long rr;
	u_long perm;
	size_t sizeMax = ROUND(size, BY2PG);
	p =                              ;
	if (p == NULL)
	{
		printf("alloc shared page failed\n");
		return NULL;
	}
	printf("alloc shared page success\n");
	void *result =                       ;
	printf("insert shared page success\n");
	return result;
}

//创建进程
int sys_env_create(int sysno, char *binary, int pt, char *arg)
{
	for (int i = 0; i < BUFLEN; i++)
	{
		// TODO

	}
	if (arg)
	{
		int i = 0;
		for (i = 0; arg[i]; i++)
		{
              //TODO			

		}
		myargv[i] = 0;
		printf("\n");
		for (i = 0; binary[i]; i++)
		{
			//TODO

		}
		printf("\n");
		myelf[i] = 0;
		printf("env_create_arg(%s,%d,%s)\n", myelf, pt, myargv);
		env_create_priority_arg(myelf, pt, myargv);
	}
	else
	{
		for (int i = 0; binary[i]; i++)
		{
			//TODO

		}
		printf("\n");
		printf("env_create(%s,%d)\n", myelf, pt);
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
int sys_set_pgfault_handler(int sysno, u_int envid, u_int func, u_int xstacktop)
{
	
	struct Env *env;
	int ret;
	ret = envid2env(envid, &env, 0);
	if (ret < 0)
	{
		printf("sys_set_pgfault_handler:can't get env");
		return -E_INVAL;
	}
	// Your code here.



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
int sys_mem_alloc(int sysno, u_int envid, u_int va, u_int perm)
{
	struct Env *env;
	struct Page *ppage;
	int ret;
	ret = 0;
	// check whether permission is legal
	


	// check whether va is legal
	



	// try to alloc a page
	ret =        ;
	if (ret < 0)
	{
		printf("sys_mem_alloc:failed to alloc a page\n");
		return -E_NO_MEM;
	}
	//try to check and get the env_id;
	ret =        ;
	if (ret < 0)
	{
		printf("sys_mem_alloc:failed to get the target env\n");
		return -E_BAD_ENV;
	}
	//now insert
	ret =         ;
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
	if (                    )
	{ //=============================
		printf("sys_mem_map:srcenv doesn't exist\n");
		return         ;
	}
	if (                       )
	{ //==============================
		printf("sys_mem_map:dstenv doesn't exist\n");
		return  ;
	} 

	//va<UTOP?
	if (                               )
	{
		printf("sys_mem_map:va is invalid\n");
		return               ;
	}
	// perm is valid?
	if (                    )
	{
		printf("sys_mem_map:permission denied\n");
		return                ;
	}
	//try to get the page
	ppage = page_lookup(            );
	if (ppage == NULL)
	{
		printf("sys_mem_map:page of srcva is invalid\n");
		return    ;
	}
	//try to insert the page
	ret = page_insert(              );
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
int sys_mem_unmap(int sysno, u_int envid, u_int va)
{
	int ret = 0;
	struct Env *env;
	ret = (                       ); 
	if (ret < 0)
	{
		printf("sys_mem_alloc:failed to get the target env\n");
		return -E_BAD_ENV;
	}
	if (va >= UTOP)
	{
		printf("sys_mem_unmap:va is not valid\n");
		return -E_NO_MEM;
	}
	page_remove(                      );
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
int sys_set_env_status(int sysno, u_int envid, u_int status)
{
	struct Env *env;
	int r;
	extern int cur_sched;

	if (                          )
	{
		printf("set_env_status:wrong status");
		return    ;
	}
	r =                           ;
	if (r < 0)
	{
		printf("set_status:env is invalid\n");
		return        ;
	}
	if ()
	{
		
	}
	else if ()
	{
		
	}
	env->env_status =                ;
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
int sys_set_trapframe(int sysno, u_int envid, struct Trapframe *tf)
{
	//TODO
	int ret;
	struct Env *e;
	ret =                  ;
	if (ret < 0)
	{
		return ret;
	}
	e->env_tf =            ;
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
void sys_panic(int sysno, char *msg)
{
	// no page_fault_mode -- we are trying to panic!
	panic("%s", TRUP(msg));
}

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
int sys_ipc_can_send(int sysno, u_int envid, u_int value, u_int srcva,
					 u_int perm)
{

	int r;
	struct Env *e;
	struct Page *p;
	Pte *ppte;
	extern int cur_sched;
	
	//try to get the destination env
	r =                      ;
	if (r < 0)
	{
		printf("ipc_send:dstenv is invalid\n");
		return -E_BAD_ENV;
	}
	// check whether target env is requesting ipc
	if (                                    )
	{
		//	printf("ipc_send:target env id not requesting recving\n");
		return                ;
	}
	//check whether source & target virtual address is valid
	if (               )
	{
		printf("ipc_send:virtual address greater than UTOP\n");
		return -E_NO_MEM;
	}
	//try to get the page which will be sent later
	if (srcva != 0)
	{
		p =                    ;
		if (p == NULL)
		{
			printf("ipc_send:destinated  page not exist");
			return -E_NO_MEM;
		}
		r =                    ;
		if (r < 0)
		{
			printf("ipc_send:page_insert failed\n");
			return -E_NO_MEM;
		}
	}
	e->env_ipc_value =              ;
	e->env_ipc_from =               ;
	e->env_ipc_perm =               ;
	e->env_ipc_recving =            ;
	e->env_status =                 ;
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
void sys_ipc_recv(int sysno, u_int dstva)
{

	extern int remaining_time;
	void *src =      ;
	void *dst =      ;
	if (dstva >= UTOP)
	{
		printf("ipc_recv:dstva is greater than UTOP");
		return;
	}
	curenv->env_status =       ;
	curenv->env_ipc_recving =  ;
	curenv->env_ipc_dstva =    ;

	remaining_time =           ;
	bcopy(src, dst, sizeof(struct Trapframe));
	sched_yield();
}

//释放自己,目前env_free(), 由于tlb_invalidate接口问题只能接受curenv
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
int sys_write_dev(int sysno, u_int va, u_int dev, u_int len)
{
	// Your code here
	u_int startaddr = dev;
	u_int endaddr = dev + len;
	if (           )
	{
		printf("sys_write_dev:invalid dev\n");
		return -E_INVAL;
	}
	startaddr += 0xa0000000;
	bcopy(va, startaddr, len);
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
int sys_read_dev(int sysno, u_int va, u_int dev, u_int len)
{
	// Your code here
	u_int startaddr = dev;
	u_int endaddr = dev + len;
	if (                                )
	{
		printf("sys_read_dev:invalid dev\n");
		return -E_INVAL;
	}
	startaddr += 0xa0000000;
	bcopy(startaddr, va, len);
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
	return readline(prompt, ret, getargv);
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
	uint32_t *read_file_buf = 0x89a00000;
	char *str = f_gets(read_file_buf, 1024, &fil);
	str[1023] = "\0";

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
