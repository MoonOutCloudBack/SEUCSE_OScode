INCLUDES := -I./ -I../ -I../inc/
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -std=gnu11 -fno-builtin -c $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c -fno-builtin $<

.PHONY: clean

all: env_asm.o env.o sched.o hash.o

clean:
	rm -rf *~ *.o


include ../include.mk
