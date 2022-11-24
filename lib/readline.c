#include <inc/printf.h>
#include <inc/error.h>
#include <inc/string.h>
#include <drivers/console.h>
#include <inc/env.h>

#define BUFLEN 1024
#define COMMANDSNUM 15
#define NULL ((void *)0)
static char buf[BUFLEN] = {0};
static char buf_his[COMMANDSNUM][BUFLEN] = {0}; //0:earliest COMMANDSNUM-1:latest
static int cur = 0, new = 0, ddl = 0;
extern char myargv[BUFLEN];
// 上 91 65
// 下 91 66
//CTRL+Z 26
//CTRL+C 3
//CTRL+D 4
//CTRL+U 21
void readline(const char *prompt, char *ret, int getargv)
{
	if (getargv)
	{
		int i = 0;
		for (i = 0; i < BUFLEN - 1 && myargv[i]; i++)
			ret[i] = myargv[i];
		ret[i] = 0;
		return ret;
	}
	int i, c, echoing;
	if (prompt != NULL)
		printf("%s", prompt);
	bool flag91 = false;
	i = 0;
	echoing = iscons(0);

	while (1)
	{
		c = getchar();
		if (c < 0)
		{
			printf("read error: %e\n", c);
			return NULL;
		}
		else if (flag91 && c > 64 && c < 69)
		{
			while (echoing && i > 0)
			{
				cputchar('\x7f');
				i--;
			}
			if (c == 65)
			{
				if (ddl == COMMANDSNUM - 1)
				{
					if (!cur)
						cur = ddl;
					else
						cur--;
				}
				else
				{
					if (cur)
						cur--;
				}
				while (buf_his[cur][i] >= 32 && i < BUFLEN - 1)
				{
					if (echoing)
						cputchar(buf_his[cur][i]);
					buf[i] = buf_his[cur][i];
					i++;
				}
			}
			else if (c == 66)
			{
				if (cur == COMMANDSNUM - 1)
					cur = 0;
				else if (cur < ddl)
					cur++;
				while (buf_his[cur][i] >= 32 && i < BUFLEN - 1)
				{
					if (echoing)
						cputchar(buf_his[cur][i]);
					buf[i] = buf_his[cur][i];
					i++;
				}
			}
		}
		else if (c == '\b' || c == '\x7f')
		{
			if (echoing && i > 0)
			{
				cputchar('\x7f');
				i--;
			}
		}
		else if (c >= 32 && i < BUFLEN - 1)
		{
			if (echoing)
				cputchar(c);
			buf[i++] = c;
			if (c == 91)
				flag91 = true;
		}
		else if (c == '\n' || c == '\r')
		{
			if (echoing)
			{
				cputchar('\n');
			}
			buf[i] = 0;
			if (i)
			{
				memcpy((char *)buf_his[new], (char *)buf, BUFLEN);
				new == COMMANDSNUM - 1 ? new = 0 : new ++;
				ddl < COMMANDSNUM - 1 ? ddl++ : ddl;
				cur = new;
			}

			for (int i = 0; i < BUFLEN; i++)
				ret[i] = buf[i];
			return ret;
		}
	}
}
