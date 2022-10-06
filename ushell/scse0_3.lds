OUTPUT_ARCH(mips)
/*__entry = DEFINED(__reset_vector) ? 0xbfc00000 : _start;
ENTRY(__entry)
PROVIDE (__flash_start = DEFINED(__reset_vector) ? 0xbfc00000 : __app_start);
PROVIDE (__tlb_stride_length = 1);*/

__entry = main;
ENTRY(__entry)
SECTIONS
{ 

    /* Start of bootrom */

  . = 0x15000000;
  .text : {
        *(.text)
        . = ALIGN(4k);
    }

  .data : {
	*(.data)
	}

  .sdata : {
    *(.sdata)
  }

  _fbss = .;
  .bss  : {
   *(.bss)
   }

   end = . ;
   _end = . ;
}
