#include "../fs/ff.h"
#include "../inc/printf.h"
extern FATFS FatFs;   // Work area (file system object) for logical drive

extern void device_init();
extern void sys_init();

int main()
{
	//my_fs_init();
	device_init(); // initialize the devices
	//initialize the flie system
	printf("\t\t[ UART and VGA test ]\t\t\n");
	if(f_mount(&FatFs, "", 1)) {
        printf("Fail to mount SD driver!\n\r", 0);    
    }
	//SD_TEST();
  	sys_init();	// initialize the OS

	return 0;
}


