#include <inc/printf.h>
#include <inc/print.h>
#include <drivers/console.h>

/*
关于 valist：
https://blog.csdn.net/ma2595162349/article/details/102887208
是一种用来包装多个参数的宏

*/


// 非常重要的输出函数
void myoutput(void *arg, char *s, int l)
{
	int i;
	// special termination call
	if ((l == 1) && (s[0] == '\0')) {
		return;
	}

	for (i = 0; i < l; i++) {
		cputchar(s[i]);
	}
}

void printf(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	lp_Print(myoutput, 0, fmt, ap);
	va_end(ap);
}

void _panic(const char *file, int line, const char *fmt, ...)
{
	va_list ap;
  
	va_start(ap, fmt);
	printf("panic at %s:%d: ", file, line);
	lp_Print(myoutput, 0, (char *)fmt, ap);
	printf("\n");
	va_end(ap);

	for (;;);
}
