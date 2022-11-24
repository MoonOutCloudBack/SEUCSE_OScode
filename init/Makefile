INCLUDES := -I../inc 

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -fno-builtin $<

.PHONY: clean

all: init.o main.o 

clean:
	rm -rf *~ *.o


include ../include.mk

