#include "console.h"

#include "uart.h"

#include "vga_print.h"

static void cons_intr(int (*proc)(void));
static void cons_putc(int c);

/***** Serial I/O code *****/
/* 
串口 IO 的代码。

串口的意思是 串行接口，数据在传输线中一位一位的传输，所以，仅用一根线就能实现双向通信。

用到了 xilinx 串口 IP 核中的以下寄存器：
- RBR：receiving buffer 数据接收缓冲寄存器
- THR：transmit holding 发送保持寄存器
- LCR：line control 线路控制寄存器
- DLL：divide latch (least significant byte)，除数锁存（最低有效字节）寄存器
- DLM：divide latch (most significant byte)，除数锁存（最高有效字节）寄存器
- IER：interrput enable 中断使能寄存器

*/


/*
对于 printf 部分，
- 首先，看 lib/printf.c 里的 printf 函数，发现 printf 是 lib/print.c 的 lp_Print 函数套壳
- lib/print.c 的 lp_Print 函数，它的主要功能是解析 %d %s 这种 format 字符串，并不是用来输出
	- 具体的，lp_Print 需要传进来一个 输出函数 的参数，然后在 lp_Print 它自己的函数体内，调用这个输出函数，来输出字符
	- 这里，传进来的输出函数是 lib/printf.c 里的 myoutput
- 然后，
	- myoutput 是 drivers/console.c（本文件）的 cputchar 套壳
	- cputchar 是本文件的 cons_putc 套壳
	- cons_putc 是本文件的 serial_putc 套壳
- serial_putc 函数：用于串口输出，大概就是往串口的输出寄存器里塞字符
- 其他函数：
	- serial_proc_data：从端口的 buffer 寄存器里取数据
	- serial_intr：cons_intr 套壳，由想中断的设备调用，用来把想告诉我们 OS 的话（已经在串口寄存器里了）填进 buffer
		- 这里的 buffer 是 input buffer，即我们使用 OS 的时候，在 OS console 输入的内容
		- 所以，数据流向是 putty -> 串口输入寄存器 -> 本文件维护的 input buffer
	- cons_getc：得到 buffer 里存放的字符串

*/
static int serial_proc_data(void) {
    if (!get_UART_DR(get_UART_LSR())) // get when data is ready
        return -1;
    return get_UART_RBR(); // 从端口的 buffer 寄存器里取数据
}

void serial_intr(void) { cons_intr(serial_proc_data); }

static void serial_putc(int c) {
    while (!get_UART_TEMT(get_UART_LSR())) ;
    set_UART_THR(c);
}

static void serial_init(void) { init_uart(); } // 在 uart.h 里

/***** General device-independent console code *****/
// Here we manage the console input buffer,
// where we stash characters received from the keyboard or serial port
// whenever the corresponding interrupt occurs.

#define CONSBUFSIZE 512

static struct {
    u8 buf[CONSBUFSIZE]; // buffer
    u32 rpos;            // read position
    u32 wpos;            // write position
} cons;

/* 
called by device interrupt routines to feed input characters
into the circular console input buffer.
cons_intr 由串口调用，用来把已经接收到的 console input 填进 buffer
*/
static void cons_intr(int (*proc)(void)) { // 参数是一个int(void)的函数指针
	// TODO
    int c;
    while ((c = (*proc)()) != -1) { // 反复调用 serial_proc_data，一个字一个字的得到
        if (c == 0) continue;
        cons.buf[cons.wpos++] = c;
        if (cons.wpos == CONSBUFSIZE) // 循环使用的 buffer
            cons.wpos = 0;
    }
}

/*
return the next input character from the console, or 0 if none waiting
返回值为下一个 console 输入的字符（虽然 return 是 int 格式）
如果没有字符输入，就返回 0
*/
int cons_getc(void) {
	// TODO
	int c;

	// poll for any pending input characters,
	// so that this function works even when interrupts are disabled
	// (e.g., when called from the kernel monitor).
	serial_intr();
	// 把串口接收到的 console input（已经在串口寄存器里了）填进 buffer

	// grab the next character from the input buffer.
	if (cons.rpos != cons.wpos) { // 如果我还没读到 你刚停笔的位置
		c = cons.buf[cons.rpos++]; // 那么我继续读
		if (cons.rpos == CONSBUFSIZE) // 循环使用的 buffer
			cons.rpos = 0;
		return c;
	}
	return 0;
}

// output a character to the console
// 把字符串 c 输出到 console
static void cons_putc(int c) {
	// 换行符处理：Windows 的换行符是 CRLF
	if (c == '\n') { // 如果要换行，那么换行 + 回车，貌似因为我们是 Windows 系统？
		serial_putc(c);
		serial_putc('\r');
	}
	else serial_putc(c);
}

// initialize the console devices
void cons_init(void) { serial_init(); vga_print_init(); }


// `High'-level console I/O.  Used by readline and cprintf.

void cputchar(int c) { cons_putc(c); vga_print_char(c); }

int getchar(void) {
	int c;
	while((c = cons_getc()) == 0) ; // do nothing
	return c;
}

int iscons(int fdnum) { return 1; } // used by readline

