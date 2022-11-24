#include <mips/cpu.h>
#include <asm/asm.h>
#include <pmap.h>
#include <env.h>
#include <printf.h>
#include <kclock.h>
#include <trap.h>
#include <../inc/types.h>
#include <../inc/rtThread.h>
#include <../inc/hash.h>
#include <../drivers/leds.h>
#include <../drivers/switches.h>
#include <../drivers/seven_seg.h> 
#include <../drivers/vga_print.h> 
#include <../drivers/console.h> 
#include <../drivers/diskio.h> 
#include <../inc/m32c0.h>

#define K_ENV 0x88000000
#define KENV_A 0x88010000
#define KENV_B 0x88020000

u32 v,t;

extern int mCONTEXT;
extern struct HashTable ht;

extern void readline(const char *prompt, char *ret, int getargv);
extern void asid_list_init();
extern u32 get_status();

extern void device_init();
extern void sys_init();
extern void tryHashTableInit(HashTable *ht);

void interface_init()
{
    printf("\n");
    printf("        ___                                 \n");
    printf("       /   |  __  __ _____ ____   _____ ____ _\n");
    printf("      / /| | / / / // ___// __ \\ / ___// __ `/\n");
    printf("     / ___ |/ /_/ // /   / /_/ // /   / /_/ / \n");
    printf("    /_/  |_|\\__,_//_/    \\____//_/    \\__,_/  \n");
    printf("       ___   ____  ___   ____                 \n");
    printf("      |__ \\ / __ \\|__ \\ / __ \\                \n");
    printf("      __/ // / / /__/ // / / /                \n");
    printf("     / __// /_/ // __// /_/ /                 \n");
    printf("    /____/\\____//____/\\____/                  \n");
    printf("\n");
}

void time_setup()
{
    char * buf;
    char * h, *m, *s;
    printf("Please setup the system time(xx:xx:xx) \n");
    readline("hour> ", h, 0);
    readline("minute> ", m, 0);
    readline("sec> ", s, 0);
    
    set_seven_seg_value(*s);
}

void device_init()
{
    cons_init();
    init_seven_seg();
    led_init();
    asid_list_init();


    mips32_bissr (SR_IE | SR_HINT0 | SR_HINT1 | SR_HINT2 | SR_HINT3 | SR_HINT4);

    enable_all_seven_seg();

    rt_device_init(LED_ID, &rt_leds_read,&rt_leds_write, &rt_leds_write_byte,3,&rt_leds_write_by_num,NULL,&rt_leds_require,&rt_leds_release);
    rt_device_init(SWITCH_ID,&rt_switches_read,NULL,NULL,1,NULL,NULL,NULL,NULL);
    rt_device_init(SEG_ID, &rt_segs_read,&rt_leds_write, &rt_segs_write_byte,4,&rt_seven_seg_write_by_num,NULL,&rt_seven_seg_require,&rt_seven_seg_release);


}

void sys_init()
{
    printf("\n");
    printf("*******Start to detect available memory space:\n");
	set_physic_mm();
    printf("\n");
    printf("*******Start to initialize the virtual memory:\n");
	vm_init();
    printf("\n");
    printf("*******Start to initialize page memory management:\n");
	page_init();

    printf("\n");
    printf("*******Start to initialize process management:\n");
	env_init();
    printf("\n");
	
    printf("*******Start to load user program:\n");

    printf("\n");
    printf("*******Start to initialize traps:\n");
	trap_init();
    printf("\n");
    printf("*******Start to initialize kclock:\n");
    
    printf("kclock init has been completed\n"); 
    printf("\n");
    printf("*******The whole system is ready!\n");
    
    interface_init();               //initialize the interface
    tryHashTableInit(&ht);

    printf("\n@@AFTER INIT: CP0_status: 0x%x\n\n",get_status());
    

    env_create_priority("ushell.elf", 0);        //shell优先级得调低
    // env_create_priority("create2.elf", 2);    // 在里面load两个elf： test1.elf和test2.elf，然后把自身释放掉
    // env_create_priority("led1.elf", 2);//亮led灯
    // env_create_priority("test_end.elf", 2);//跑一句printf就结束
    // env_create_priority("led2.elf", 2);
    // env_create_priority("led3.elf", 2);


    // env_create_priority("test1.elf", 2);//循环打印test one
    // env_create_priority("test_end.elf", 1);//跑一句printf就结束
    // env_create_priority("test2.elf", 1);//循环打印test two
    // env_create_priority("thread.elf", 2);//创建线程，循环打印test one


    // // //test 两个一起测共享内存
    // env_create_priority("share1.elf", 1);  //往共享内存页某个地址写99
    // env_create_priority("share2.elf", 2);  //读出共享内存页某个地址 输出写进去的数据


    asm ("ei");//中断使能
    
    kclock_init();  //设置中断时间长短
                    //进时间中断后，下面不会被执行到
    while(1){
        
        //rt_device_read(SWITCH_ID, &t);
        //rt_device_write(LED_ID,&t);
        //rt_device_write(SEG_ID,&t);
        //printf("in kernel %x\n",t);
    }

}



void code_test()
{
    set_leds(0xff);
}


void SD_TEST()
{
    disk_initialize(0);
}

