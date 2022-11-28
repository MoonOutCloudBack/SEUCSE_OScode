#include "shell.h"
// Simple command-line kernel shell useful for
// controlling the kernel and exploring the system interactively.
#include <rtThread.h>
//#include <printf.h>
#include <string.h>
// #include <mmu.h>
// #include <uart.h>
#include "ff.h"
// #include <fs.h>
// #include <printf.h>

// #include <console.h>
#include "lib.h"
// #include "syscall_lib.h"
// max size of file image is 16M
#define MAX_FILE_SIZE 0x1000000

// size of DDR RAM (256M for Minisys) 
#define DDR_SIZE 0x10000000

// 4K size read burst
#define SD_READ_SIZE 4096


#define CMDBUF_SIZE	80	// enough for one VGA text line
extern FATFS FatFs;   // Work area (file system object) for logical drive

struct Command {
    const char *name;
    const char *desc;
    // return -1 to force shell to exit
    int (*func)(int argc, char** argv, struct Trapframe* tf);
};

static struct Command commands[] = {
    { "help", "Display this list of commands", mon_help },
    { "ls", "List files and directories", mon_ls },
	{ "cd", "Change Directory", mon_cd },
    { "touch", "Create file", mon_touch },
    { "mkdir", "Create directory", mon_mkdir },
	{ "read", "Read a file", mon_read },
	{ "write", "Change a file", mon_write },
	{ "rm", "Delete files or directories", mon_rm } //，
};


/***** Kernel shell command interpreter *****/

#define WHITESPACE "\t\n "
#define MAXARGS 16
static char buf[1024];
static char nextcmd[1024];
static char argv_new[20];
static char elf[20];

static int runcmd(char *buf, struct Trapframe *tf)
{
	//syscall_printf("In runcmd:%s\n",buf);
    int argc,mon=0;
    char *argv[MAXARGS];
    int i;
    // {
    //     syscall_printf("buf:%x\n",buf);
    //         for(int i=0;i<35;i++) syscall_printf(" %d%c",buf[i],buf[i]);
    //     syscall_printf("\n");
    // }
    while (*buf && strchr(WHITESPACE, *buf)) *buf++ = 0;
    if(*buf=='&')buf++;
    char* andflag=strchr(buf,'&');
    buf[andflag-buf]='\0';
    //syscall_printf("%dandflag:%x\n",andflag-buf,andflag);
    if(andflag){
        for(int i=0;!i||andflag[i];i++)
            nextcmd[i]=andflag[i+1];
        //syscall_printf("nextcmd:%s\n",nextcmd);
    }
    // {
    //     syscall_printf("buf:%x\n",buf);
    //         for(int i=0;i<35;i++) syscall_printf(" %d%c",buf[i],buf[i]);
    //     syscall_printf("\n");
    // }
    argc = 0;
    argv[argc] = 0;
    while (1) {
        while (*buf && strchr(WHITESPACE, *buf)) {
            //syscall_printf("%x:%d\n",buf,*buf);
            *buf++ = 0;
        }
        if (*buf == 0) break;
        //syscall_printf("buf:%s\n",buf);
        if (argc == MAXARGS-1) {
            syscall_printf("Too many arguments (max %d)\n", MAXARGS);
            return 0;
        }
        argv[argc++] = buf;
        while (*buf && !strchr(WHITESPACE, *buf))
            buf++;
    }
    argv[argc] = 0;
    // syscall_printf("argv[0]:%s,argv[1]:%s\n",argv[0],argv[1]);
    // {
    //     syscall_printf("buf:%x\n",buf);
    //     for(int i=0;i<5;i++) syscall_printf(" %d ",buf[i]);
    //     syscall_printf("\nargv[0]:%x\n",argv[0]);
    //     for(int i=0;i<5;i++) syscall_printf(" %d ",argv[0][i]);
    // }   
    if (argc == 0) return 0;
    for (i = 0; i < ARRAY_SIZE(commands); i++) {
        if (strcmp(argv[0], commands[i].name) == 0){
            commands[i].func(argc, argv, tf);
            mon=1;
        }
    }
    if(!mon){
        // {
        //     syscall_printf("\n");
        //     for(int i=0;i<5;i++) syscall_printf(" %d ",argv[0][i]);
        //     syscall_printf("\n");
        //     syscall_printf ("run file %s\n",argv[0]);
        // }
        for (i = 0 ; argv[1][i]; i++)
            argv_new[i] = argv[1][i];
        argv_new[i] = 0;
        for (i = 0 ; argv[0][i]; i++)
            elf[i] = argv[0][i];
        elf[i] = 0;
        //syscall_env_create(argv[0],2,argv[1]);
        syscall_printf("elf:%s\n",elf);
        if (argv_new[0]){
            //syscall_printf("with argv:%s\n",argv_new);
            syscall_env_create(elf,2,argv_new);
        }
        else
            syscall_env_create(elf,2,NULL);
    }
    if(andflag){
        //syscall_printf ("&runcmd %s\n",nextcmd);
        runcmd(nextcmd,tf);
    }
    return 0;
}

void shell(struct Trapframe *tf)
{
    
    syscall_printf("Aurora, an operating system based on MIPS32\n");
    syscall_printf("Type 'help' for more commands.\n");

    while (1) {
        syscall_readline("Aurora> ",buf,0);
        if (buf != NULL)
             if (runcmd(buf, tf) < 0)
                 break;
    }
}

/***** Implementations of basic kernel shell commands *****/

int mon_help(int argc, char **argv, struct Trapframe *tf)
{
    for (int i = 0; i < ARRAY_SIZE(commands); i++)
        {
			//printf("%s - %s\n", commands[i].name, commands[i].desc);
			syscall_printf(commands[i].name);
			syscall_printf(" - ");
			syscall_printf(commands[i].desc);
			syscall_printf("\n");
		}
    return 0;
}

/***** File system operations *****/
int mon_ls(int argc, char **argv, struct Trapframe *tf)
{
	return syscall_ls();
}

int mon_touch(int argc, char **argv, struct Trapframe *tf)
{
	return syscall_fcraete(argv[1]);
}

int mon_cd(int argc, char **argv, struct Trapframe *tf)
{
	return syscall_cd(argv[1]);
}

int mon_mkdir(int argc, char **argv, struct Trapframe *tf)
{
	return syscall_mkdir(argv[1]);
}

int mon_rm(int argc, char **argv, struct Trapframe *tf)
{
	return syscall_rm(argv[1]);
}

int mon_read(int argc, char **argv, struct Trapframe *tf){
	return syscall_fread(argv[1]);
}

int mon_write(int argc, char **argv, struct Trapframe *tf){
	return syscall_fwrite(argv[1], argv[2]);
}

char* Int2String(int num,char *str)//10进制 
{
    int i = 0;//指示填充str 
    if(num<0)//如果num为负数，将num变正 
    {
        num = -num;
        str[i++] = '-';
    } 
    //转换 
    do
    {
        str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0' 
        num /= 10;//去掉最低位    
    }while(num);//num不为0继续循环
    
    str[i] = '\0';
    
    //确定开始调整的位置 
    int j = 0;
    if(str[0]=='-')//如果有负号，负号不用调整 
    {
        j = 1;//从第二位开始调整 
        ++i;//由于有负号，所以交换的对称轴也要后移1位 
    }
    //对称交换 
    for(;j<i/2;j++)
    {
        //对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b; 
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    } 
    
    return str;//返回转换后的值 
}

void test_end()
{
	syscall_printf("######## i am dying ############\n");
}


void test1()
{
	int i=1;
	while(i++)
	{
		if(i%1000==0)
		{
			syscall_printf("test one :%d\n",i);
			if(i==10000) i=1;
		}
		
	}
}

void test2()
{
	int i=1;
	while(i++)
	{
		if(i%1000==0)
		{
			syscall_printf("test two :%d\n",i);
			if(i==10000) i=1;
		}
		
	}
}

void create2()
{
    syscall_printf(" *************** test test_shell_env2 *************** \n");
    syscall_env_create("test1.elf",2,NULL);
    syscall_env_create("test2.elf",2,NULL);
    syscall_printf(" *************** test test_shell_env2 *************** \n");
}



void led1()
{
	while(1)
	{
		syscall_set_leds(0x00FFF000);
	}
}

void led2()
{
	while(1)
	{
		syscall_set_leds(0x00000FFF);
	}
}

void led3()
{
	while(1)
	{
		syscall_set_leds(0x0000FF00);
	}
}

void share1()
{
    int* share_pg = syscall_get_shm(1,4096);
    *(share_pg+4) = 99;
    while(1)
    {
        
    }
}

void share2()
{
    int* share_pg = syscall_get_shm(1,4096);
    int result = *(share_pg+4);
    syscall_printf("test three :%d\n",result);
    while(1)
    {
        
    }
}

void thread()
{
    syscall_printf(" *************** test pthread *************** \n");
    syscall_pthread_create(test1,1);
    syscall_printf(" *************** test pthread end *************** \n");

}

int test_banker(){
     syscall_printf("in RT.elf");
    char buf = 10;
    char buf1 = 11;
    
    u32 device [2];
    device[0] = 2;
    device[1] = 2;

    if (syscall_rt_claim_device(device))
        syscall_printf("rt claim device success\n");
    if (syscall_rt_require_device(LED_ID, 2))
        syscall_printf("rt require LED success\n");
    if (syscall_rt_require_device(SEG_ID, 1))
        syscall_printf("rt require SEG success\n");
    if (syscall_rt_write_by_num(LED_ID, 0, &buf))
        syscall_printf("rt write LED1 success\n");
    if (syscall_rt_write_by_num(LED_ID, 1, &buf1))
        syscall_printf("rt write LED2 success\n");
    if (syscall_rt_write_by_num(SEG_ID,0, &buf));
        syscall_printf("rt write SEG success\n");
    syscall_rt_release_device(LED_ID, 1);
    syscall_rt_release_device(SEG_ID, 1);

    syscall_rt_exit();
    return 0;
}