// Basic string routines. Not hardware optimized, but not shabby.

#include <string.h>

int strlen(const char *s) {
	int n;

	for (n = 0; *s != '\0'; s++)
		n++;
	return n;
}

int strnlen(const char *s, size_t size) {
	int n;

	for (n = 0; size > 0 && *s != '\0'; s++, size--)
		n++;
	return n;
}

char *strcpy(char *dst, const char *src) {
	char *ret;

	ret = dst;
	while ((*dst++ = *src++) != '\0')
		/* do nothing */;
	return ret;
}

char *strcat(char *dst, const char *src) {
	int len = strlen(dst);
	strcpy(dst + len, src);
	return dst;
}

char *strncpy(char *dst, const char *src, size_t size) {
	size_t i;
	char *ret;

	ret = dst;
	for (i = 0; i < size; i++) {
		*dst++ = *src;
		// If strlen(src) < size, null-pad 'dst' out to 'size' chars
		if (*src != '\0')
			src++;
	}
	return ret;
}

size_t strlcpy(char *dst, const char *src, size_t size) {
	char *dst_in;

	dst_in = dst;
	if (size > 0) {
		while (--size > 0 && *src != '\0')
			*dst++ = *src++;
		*dst = '\0';
	}
	return dst - dst_in;
}

int strcmp(const char *p, const char *q) {
	while (*p && *p == *q)
		p++, q++;
	return (int) ((unsigned char) *p - (unsigned char) *q);
}

int strncmp(const char *p, const char *q, size_t n) {
	while (n > 0 && *p && *p == *q)
		n--, p++, q++;
	if (n == 0)
		return 0;
	else
		return (int) ((unsigned char) *p - (unsigned char) *q);
}

// Return a pointer to the first occurrence of 'c' in 's',
// or a null pointer if the string has no 'c'.
char *strchr(const char *s, int c) {
	for (; *s; s++)
		if (*s == c)
			return (char *) s;
	return 0;
}

// Return a pointer to the first occurrence of 'c' in 's',
// or a pointer to the string-ending null character if the string has no 'c'.
char *strfind(const char *s, char c) {
	for (; *s; s++)
		if (*s == c)
			break;
	return (char *) s;
}

void *memset(void *v, int c, size_t n) {
	char *p;
	int m;

	p = v;
	m = n;
	while (--m >= 0)
		*p++ = c;

	return v;
}

void *memmove(void *dst, const void *src, size_t n) {
	const char *s;
	char *d;

	s = src;
	d = dst;
	if (s < d && s + n > d) {
		s += n;
		d += n;
		while (n-- > 0)
			*--d = *--s;
	} else
		while (n-- > 0)
			*d++ = *s++;

	return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
	return memmove(dst, src, n);
}

int memcmp(const void *v1, const void *v2, size_t n) {
	const uint8_t *s1 = (const uint8_t *) v1;
	const uint8_t *s2 = (const uint8_t *) v2;

	while (n-- > 0) {
		if (*s1 != *s2)
			return (int) *s1 - (int) *s2;
		s1++, s2++;
	}

	return 0;
}

void *memfind(const void *s, int c, size_t n) {
	const void *ends = (const char *) s + n;
	for (; s < ends; s++)
		if (*(const unsigned char *) s == (unsigned char) c)
			break;
	return (void *) s;
}

// Convert string to long integer
long strtol(const char *s, char **endptr, int base)
{
	int neg = 0;
	long val = 0;

	// gobble initial whitespace
	while (*s == ' ' || *s == '\t')
		s++;

	// plus/minus sign
	if (*s == '+')
		s++;
	else if (*s == '-')
		s++, neg = 1;

	// hex or octal base prefix
	if ((base == 0 || base == 16) && (s[0] == '0' && s[1] == 'x'))
		s += 2, base = 16;
	else if (base == 0 && s[0] == '0')
		s++, base = 8;
	else if (base == 0)
		base = 10;

	// digits
	while (1) {
		int dig;

		if (*s >= '0' && *s <= '9')
			dig = *s - '0';
		else if (*s >= 'a' && *s <= 'z')
			dig = *s - 'a' + 10;
		else if (*s >= 'A' && *s <= 'Z')
			dig = *s - 'A' + 10;
		else
			break;
		if (dig >= base)
			break;
		s++, val = (val * base) + dig;
		// we don't properly detect overflow!
	}

	if (endptr)
		*endptr = (char *) s;
	return (neg ? -val : val);
}

size_t strspn(const char*s1,const char *s2){
	int i,j;
	for(i=0;;i++){
		for(j=0;s2[j];j++){
			if(s1[i]==s2[j]) break;
		} 
		if(s1[i]==NULL||s2[j]==NULL) break;
	}
	return i;
}
char * strpbrk(const char*s1,const char *s2){
	int i,j;
	const char*rt;
	for(rt=s1;*rt!=NULL;rt++){
		for(j=0;s2[j]!=NULL;j++){
			if(*rt==s2[j]) return(char *)rt;
		}
	}
	return NULL;
}
char* strtok_inside(char* str_arr,const char* delimiters,char**temp_str)
{
    
    char*b_temp;

    if(str_arr == NULL) str_arr =*temp_str;

    str_arr += strspn(str_arr, delimiters);

    if(*str_arr =='\0') return NULL;
    /*
    * 保存当前的待分解串的指针b_temp，调用strpbrk()在b_temp中找分解符，
    * 如果找不到，则将temp_str赋值为待分解字符串末尾部'\0'的位置，
    * b_temp没有发生变化；若找到则将分解符所在位置赋值为'\0',
    * b_temp相当于被截断了，temp_str指向分解符的下一位置。
    */
    b_temp = str_arr;
    str_arr = strpbrk(str_arr, delimiters);
    if(str_arr == NULL)
    {
        *temp_str = strchr(b_temp,'\0');
    }
    else
    {
        *str_arr ='\0';
        *temp_str = str_arr +1;
    }
    //5、函数最后部分无论找没找到分解符，都将b_temp返回。
    return b_temp;
} 


char* strtok(char* str_arr,const char* delimiters)
{
    static char*last;
    return strtok_inside(str_arr, delimiters, &last);
}