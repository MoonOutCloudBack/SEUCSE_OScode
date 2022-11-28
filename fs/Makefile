INCLUDES	  := -I../inc/

all:  elf.o ff.o 

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES)  -fno-builtin -c $< -o $*.o


clean:
	rm -rf *~ *.o

include ../include.mk
