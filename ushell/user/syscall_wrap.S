#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>


LEAF(msyscall)
    # // TODO: execute a `syscall` instruction and return from msyscall
	
	nop
	ehb
	nop
	syscall
	nop
	nop
	jr ra
	nop
END(msyscall)

