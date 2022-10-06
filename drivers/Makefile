# Makefile for gxconsole module
#
# Copyright (C) 2007 Beihang Unversity.
# Written by Zhu Like, zlike@cse.buaa.edu.cn

# ========== Configuration =============

# CROSS_COMPILE := /opt/eldk/usr/bin/mips_4KC-
# CC			  := $(CROSS_COMPILE)gcc
# CFLAGS		  := -O -G 0 -mno-abicalls -fno-builtin -Wall

# ========= End of configuration =======
INCLUDES	  := -I../inc/

all: buzzer.o diskio.o leds.o sd.o seven_seg.o switches.o timer.o uart.o console.o button.o vga_print.o

%.o: %.c %.h
	$(CC) $(CFLAGS) $(INCLUDES) -std=c11 -c $< -o $*.o
clean:
	rm -rf *~ *.o

include ../include.mk
