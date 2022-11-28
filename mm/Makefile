INCLUDES	:= -I./ -I../ -I../inc/ 

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -std=gnu11 -c -fno-builtin $<

%.o: %.S
	$(CC) $(CFLAGS) $(INCLUDES) -c -fno-builtin $<

.PHONY: clean

all: pmap.o tlb_asm.o m32tlb_ops.o tlbop.o

clean:
	rm -rf *~ *.o


include ../include.mk
