/* See COPYRIGHT for copyright information. */

/* The Run Time Clock and other NVRAM access functions that go with it. */
/* The run time clock is hard-wired to IRQ8. */

#include <kclock.h>
#include <kclock.h>

extern void init_timer();

void
kclock_init(void)
{
	
	//set_timer();
	init_timer();
}

