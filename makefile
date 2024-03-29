drivers_dir	  := drivers
boot_dir	  := boot
init_dir	  := init
lib_dir		  := lib
fs_dir		  := fs
env_dir		  := env
mm_dir		  := mm
tools_dir	  := tools
kernel_dir	  := kernel
link_script   := scse0_3.lds

modules		  := boot drivers init lib mm env fs 
objects		  := $(boot_dir)/*.o			  \
			 	 $(lib_dir)/*.o				  \
				 $(drivers_dir)/*.o	 		  \
				 $(init_dir)/*.o			  \
				 $(mm_dir)/*.o				  \
				 $(env_dir)/*.o 			  \
				 $(fs_dir)/*.o


.PHONY: all $(modules) clean run

all: $(modules) vmlinux

vmlinux: $(modules)
	$(LD) -EL -nostartfiles -N -T scse0_3.lds -O0 -G0 -o vmlinux.elf $(objects) 
	$(OC) --remove-section .MIPS.abiflags --remove-section .reginfo vmlinux.elf
	$(SZ) vmlinux.elf
	$(OD) -D -l -t vmlinux.elf > vmlinux.dis
	$(OD) -D vmlinux.elf > vmlinux.txt
	$(OC) vmlinux.elf -O srec vmlinux.rec

$(modules): 
	$(MAKE) --directory=$@

clean: 
	for d in $(modules);	\
		do					\
			$(MAKE) --directory=$$d clean; \
		done; \
	rm -rf *.o *~ $(vmlinux_elf)  $(user_disk)


include include.mk
