#include <mips/cpu.h>
#include <../inc/string.h>
void bcopy(const void *src, void *dst, size_t len)
{
	void *max;
	max = dst + len;
	int iii = 0;
	while (dst + 3 < max) {
		*(int *)dst = *(int *)src;
		dst += 4;
		src += 4;
	}
	while (dst < max) {
		*(char *)dst = *(char *)src;
		dst += 1;
		src += 1;
	}
}

void bzero(void *b, size_t len)
{
	void *max;
	max = b + len;
	while (b + 3 < max) {
		*(int *)b = 0;
		b += 4;
	}
	while (b < max) {
		*(char *)b++ = 0;
	}
}
