#include "lib.h"
#include <unistd.h>
#include <trap.h>

void syscall_putchar(char ch)
{
	msyscall(SYS_putchar, (int)ch, 0, 0, 0, 0);
}

u_int syscall_getenvid(void)
{
	return msyscall(SYS_getenvid, 0, 0, 0, 0, 0);
}

// 共享内存系统调用  
void* syscall_get_shm(int key, int size)
{
	return msyscall(SYS_get_shm, key, size, 0, 0, 0);
}
// load elf创建进程的系统调用
void syscall_env_create(char* binary,int pt,char*argv)
{
	msyscall(SYS_env_create, binary, pt, argv, 0, 0);
}

int syscall_set_pgfault_handler(u_int envid, void (*func)(void), u_int xstacktop)
{
	return msyscall(SYS_set_pgfault_handler, envid, (int)func, xstacktop, 0, 0);
}

int syscall_mem_alloc(u_int envid, u_int va, u_int perm)
{
	return msyscall(SYS_mem_alloc, envid, va, perm, 0, 0);
}

int syscall_mem_map(u_int srcid, u_int srcva, u_int dstid, u_int dstva, u_int perm)
{
	return msyscall(SYS_mem_map, srcid, srcva, dstid, dstva, perm);
}

int syscall_mem_unmap(u_int envid, u_int va)
{
	return msyscall(SYS_mem_unmap, envid, va, 0, 0, 0);
}

//创建线程
void syscall_pthread_create(void *func, int arg)
{
	msyscall(SYS_pthread_create, func, arg, 0, 0, 0);
}

int syscall_set_env_status(u_int envid, u_int status)
{
	return msyscall(SYS_set_env_status, envid, status, 0, 0, 0);
}

int syscall_set_trapframe(u_int envid, struct Trapframe *tf)
{
	return msyscall(SYS_set_trapframe, envid, (int)tf, 0, 0, 0);
}

void syscall_panic(char *msg)
{
	msyscall(SYS_panic, (int)msg, 0, 0, 0, 0);
}

int syscall_ipc_can_send(u_int envid, u_int value, u_int srcva, u_int perm)
{
	return msyscall(SYS_ipc_can_send, envid, value, srcva, perm, 0);
}

void syscall_ipc_recv(u_int dstva)
{
	msyscall(SYS_ipc_recv, dstva, 0, 0, 0, 0);
}

int syscall_free_myself()
{
	return msyscall(SYS_free_myself, 0, 0, 0, 0, 0);
}

int syscall_write_dev(u_int va, u_int dev, u_int len)
{
	return msyscall(SYS_write_dev, va, dev, len, 0, 0);
}


int syscall_read_dev(u_int va, u_int dev, u_int len)
{
	return msyscall(SYS_read_dev, va, dev, len, 0, 0);
}

void syscall_printf(char *fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
	msyscall(SYS_printf, fmt, &ap, 0, 0, 0);
}

void syscall_set_leds(u32 val)
{
	msyscall(SYS_set_leds, val, 0, 0, 0, 0);
}

u32 syscall_get_switchs(void)
{
	msyscall(SYS_get_switchs, 0, 0, 0, 0, 0);
}

void syscall_readline(const char *prompt,char *ret,int getargv)
{
	msyscall(SYS_readline, prompt, ret, getargv, 0, 0);
}
//换个别的
void syscall_env_create_1(u32 env_va)
{
	msyscall(SYS_env_create_1, env_va, 0, 0, 0, 0);
}
    
int syscall_mkdir(char *path)
{
	msyscall(SYS_mkdir, path, 0, 0, 0, 0);
}
int syscall_cd(char *path)
{
	msyscall(SYS_cd, path, 0, 0, 0, 0);
}
int syscall_fcraete(char *fname)
{
	msyscall(SYS_fcraete, fname, 0, 0, 0, 0);
}
int syscall_fread(char *path)
{
	msyscall(SYS_fread, path, 0, 0, 0, 0);
}
int syscall_fwrite(char *path, char *str)
{
	msyscall(SYS_fwrite, path, str, 0, 0, 0);
}
int syscall_ls()
{
	msyscall(SYS_ls, 0, 0, 0, 0, 0);
}
int syscall_rm(char* path)
{
	msyscall(SYS_rm, path, 0, 0, 0, 0);
}

int syscall_rt_write_byte(u32 device_id,char *buf,u32 i)
{
	msyscall(SYS_rt_write_byte, device_id, buf, i, 0, 0);
}
int syscall_rt_require_device(u32 device_id, u32 num)
{
	msyscall(SYS_rt_require_device, device_id, num, 0, 0, 0);
}
int syscall_rt_release_device(u32 device_id, u32 num)
{
	msyscall(SYS_rt_release_device, device_id, num, 0, 0, 0);
}
int syscall_rt_claim_device(u32 * req)
{
	msyscall(SYS_rt_claim_device, req, 0, 0, 0, 0);
}
int syscall_rt_write_by_num(u32 device_id, u32 num, char *buf)
{
	msyscall(SYS_rt_write_by_num, device_id, num, buf, 0, 0);
}

int syscall_rt_exit()
{
	msyscall(SYS_rt_exit,0,0,0,0,0);
}