user_dir	  := user
init_dir	  := init
link_script   := scse0_3.lds

modules		  := user init  
objects		  := $(user_dir)/*.o	 		  \
				 $(init_dir)/*.o			  
				 
.PHONY: all $(modules) clean run

all: $(modules) ushell

ushell: $(modules)
	$(LD) -EL -nostartfiles -N -T scse0_3.lds -G0 -o ushell.elf $(objects) 
	$(OC) --remove-section .MIPS.abiflags --remove-section .reginfo ushell.elf
	$(SZ) ushell.elf
	$(OD) -D -l -t ushell.elf > ushell.dis
	$(OD) -D ushell.elf > ushell.txt
	$(OC) ushell.elf -O srec ushell.rec

$(modules): 
	$(MAKE) --directory=$@

clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ $(ushell_elf)  $(user_disk)


include include.mk
