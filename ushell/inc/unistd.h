#ifndef UNISTD_H
#define UNISTD_H

#define __SYSCALL_BASE 9527
#define __NR_SYSCALLS 35


#define SYS_putchar 		((__SYSCALL_BASE ) + (0 ) ) 
#define SYS_getenvid 		((__SYSCALL_BASE ) + (1 ) )
#define SYS_get_shm			((__SYSCALL_BASE ) + (2 ) )
#define SYS_env_create		((__SYSCALL_BASE ) + (3 ) )
#define SYS_set_pgfault_handler	((__SYSCALL_BASE ) + (4 ) )
#define SYS_mem_alloc		((__SYSCALL_BASE ) + (5 ) )
#define SYS_mem_map			((__SYSCALL_BASE ) + (6 ) )
#define SYS_mem_unmap		((__SYSCALL_BASE ) + (7 ) )
#define SYS_pthread_create		((__SYSCALL_BASE ) + (8 ) )
#define SYS_set_env_status	((__SYSCALL_BASE ) + (9 ) )
#define SYS_set_trapframe	((__SYSCALL_BASE ) + (10 ) )
#define SYS_panic			((__SYSCALL_BASE ) + (11 ) )
#define SYS_ipc_can_send	((__SYSCALL_BASE ) + (12 ) )
#define SYS_ipc_recv		((__SYSCALL_BASE ) + (13 ) )
#define SYS_free_myself			((__SYSCALL_BASE ) + (14 ) )
#define SYS_write_dev		((__SYSCALL_BASE ) + (15 ) )
#define SYS_read_dev		((__SYSCALL_BASE ) + (16 ) )
#define SYS_printf          ((__SYSCALL_BASE ) + (17 ) )
#define SYS_set_leds        ((__SYSCALL_BASE ) + (18 ) )
#define SYS_get_switchs     ((__SYSCALL_BASE ) + (19 ) )
#define SYS_readline        ((__SYSCALL_BASE ) + (20 ) )
#define SYS_env_create_1      ((__SYSCALL_BASE ) + (21 ) )
#define SYS_mkdir           ((__SYSCALL_BASE ) + (22 ) )
#define SYS_cd              ((__SYSCALL_BASE ) + (23 ) )
#define SYS_fcraete         ((__SYSCALL_BASE ) + (24 ) )   
#define SYS_fread           ((__SYSCALL_BASE ) + (25 ) )
#define SYS_fwrite          ((__SYSCALL_BASE ) + (26 ) )
#define SYS_ls              ((__SYSCALL_BASE ) + (27 ) )
#define SYS_rm              ((__SYSCALL_BASE ) + (28 ) )
#define SYS_rt_write_byte   ((__SYSCALL_BASE ) + (29 ) )
#define SYS_rt_require_device ((__SYSCALL_BASE ) + (30 ) )
#define SYS_rt_release_device ((__SYSCALL_BASE ) + (31 ) )
#define SYS_rt_claim_device  ((__SYSCALL_BASE ) + (32 ) )
#define SYS_rt_write_by_num  ((__SYSCALL_BASE ) + (33 ) )
#define SYS_rt_exit          ((__SYSCALL_BASE ) + (34 ) )

#endif
