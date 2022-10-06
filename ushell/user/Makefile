INCLUDES = -I../inc/ -I../drivers/ -I../fs/

#CROSS_COMPILE := /usr/eldk/usr/bin/mips_4KC-
#CC           := $(CROSS_COMPILE)gcc
#CFLAGS       := -O -G 0 -mno-abicalls -fno-builtin -Wa,-xgot -Wall -fPIC
#LD           := $(CROSS_COMPILE)ld
# For embedding one program in another
OBJDIRS := ../boot ../lib ../drivers ../init ../mm ../tool ../fs
ASOURCES = $(foreach dir, $(OBJDIRS), $(wildcard $(dir)/*.S))
CSOURCES = $(foreach dir, $(OBJDIRS), $(wildcard $(dir)/*.c)) idle.c
COBJECTS = $(CSOURCES:.c=.o) 
AOBJECTS = $(ASOURCES:.S=.o) 


USERLIB := syscall_lib.o \
		syscall_wrap.o \
		shell.o 	\
		string.o
		

CFLAGS += -nostdlib -static

# all: idle.elf
all: ${USERLIB}
# all: echo.x echo.b  num.x num.b testptelibrary.b testptelibrary.x fktest.x fktest.b pingpong.x pingpong.b idle.x testarg.b testpipe.x testpiperace.x icode.x init.b sh.b cat.b ls.b fstest.x fstest.b $(USERLIB) entry.o syscall_wrap.o

# %.x: %.b.c 
# 	echo cc1 $< 
# 	$(CC) $(CFLAGS) -c -o $@ $<

# %.b.c: %.b
# 	echo create $@
# 	echo bintoc $* $< > $@~
# 	chmod +x ./bintoc
# 	./bintoc $* $< > $@~ && mv -f $@~ $@
#   grep \. $@


idle.elf: entry.o syscall_wrap.o  $(USERLIB) $(COBJECTS) $(AOBJECTS)
	echo ld $@
	$(LD) -o $@ -EL -static -N -T user.lds -G0 $^
	$(OC) --remove-section .MIPS.abiflags --remove-section .reginfo idle.elf
	$(SZ) idle.elf
	$(OD) -D -l -t idle.elf > idle.dis
	$(OD) -D idle.elf > idle.txt
	$(OC) idle.elf -O srec idle.rec

%.o: %.c
	echo user1 cc $<
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: %.S
	echo as $<
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

%.o: lib.h

.PHONY: clean

clean:
	rm -rf *~ *.o *.b.c *.x *.b

include ../include.mk
