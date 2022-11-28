INCLUDES := -I./ -I../ -I../inc/
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -fno-builtin $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c -fno-builtin $<

.PHONY: clean

all: print.o printf.o kclock.o traps.o genex.o kclock_asm.o syscall.o syscall_all.o getc.o string.o readline.o tool.o rtThread.o

clean:
	rm -rf *~ *.o


include ../include.mk
