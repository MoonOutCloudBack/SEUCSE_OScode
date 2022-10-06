INCLUDES := -I./ -I../ -I../inc/
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

.PHONY: clean

all: env_asm.o env.o sched.o

clean:
	rm -rf *~ *.o


include ../include.mk
