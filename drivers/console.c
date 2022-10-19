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
cons_intr 由想中断的设备调用，用来把想说的话填进 buffer
*/
static void cons_intr(int (*proc)(void)) { // proc 是一个表示函数的 symbol
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
返回值为下一个要输出的字符（虽然 return 是 int 格式）
如果没有字符在等着输出，就返回 0
*/
int cons_getc(void) {
	// TODO
	int c;

	// poll for any pending input characters,
	// so that this function works even when interrupts are disabled
	// (e.g., when called from the kernel monitor).
	serial_intr();
	// 把想说的话（已经在串口寄存器里了）填进 buffer
	// 时刻调用一下，时刻填一下

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
	// TODO
	if (c == '\n') { // 如果要换行，那么换行 + 回车，貌似因为我们是 Windows 系统？
		serial_putc(c);
		serial_putc('\r');
	}
	else serial_putc(c);
}

// initialize the console devices
void cons_init(void) { serial_init(); vga_print_init(); }


// `High'-level console I/O.  Used by readline and cprintf.

void cputchar(int c) { cons_putc(c); }

int getchar(void) {
	int c;
	while((c = cons_getc()) == 0) ; // do nothing
	return c;
}

int iscons(int fdnum) { return 1; } // used by readline

