/*
 * boot.S for microAptiv_UP
 *
 *  Created on: April 12, 2014
 *  Author: MIPS TECHNOLOGIES, INC
 *  Start of boot code
*/
/*
Copyright (c) 2014, Imagination Technologies LLC and Imagination Technologies
Limited.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions in binary form must be built to execute on machines
   implementing the MIPS32(R), MIPS64 and/or microMIPS instruction set
   architectures.

2. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

3. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

4. Neither the name of Imagination Technologies LLC, Imagination Technologies Limited
   nor the names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL IMAGINATION TECHNOLOGIES LLC OR IMAGINATION
TECHNOLOGIES LIMITED BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#define _RESETCODE 1

#include <mips/regdef.h>
#include <mips/asm.h>
#include "boot.h"
#include "mfp_io.h"
#include <mips/m32c0.h>

.set    noreorder               # Don't allow the assembler to reorder instructions.
# 解释：默认情况下，MIPS 汇编器会尝试重新排序周围的指令，自动填充分支、加载延迟槽。
# 但是，如果我们需要保持对指令顺序的精确控制，比如在大量使用的库函数的情况下，那么 .set noreorder
# 它用来告诉汇编器，在下次遇到相应的命令之前，抑制其重新排序功能

    # The first function, __reset_vector, just loads the address of the __cpu_init function
    # and jumps to it. This does 2 things; First it will jump to a KSEG1 address which is a
    # mirror of the BEV�s KSEG0 address but cacheable. Second it will do a mode switch to
    # enable micromips mode.

/**************************************************************************************
    R E S E T   E X C E P T I O N   H A N D L E R
**************************************************************************************/

LEAF(__reset_vector)
    la a2, __cpu_init
    jr a2
                    # Clear cp0 Count (Used to measure boot time.)
    nop

END(__reset_vector)

LEAF(__cpu_init)

    # Verify the code is here due to a reset and not NMI. If this is an NMI then trigger
    # a debugger breakpoint using a sdbp instruction.

                          # Read CP0 Status
                          # extract NMI
                          # Branch if this is NOT an NMI exception.
    nop
                          # Failed assertion: NMI.



init_resources:                 # initializes resources for "cpu".

    # Initialize CP0 registers

                # Init CP0 Status, Count, Compare, Watch*, and Cause.
    
    nop

    # Initialize the TLB

               # Generate unique EntryHi contents per entry pair.
    
    nop


    # The changing of Kernel mode cacheability must be done from KSEG1
    # Since the code is executing from KSEG0 It needs to do a jump to KSEG1 change K0 and jump back to KSEG0


               # changed to KSEG1 address by setting bit 29
    
    nop
    

    # Prepare for eret to main.

            # If main returns then go to all_done:.
                      # Indicate that there are no arguments available.
              # load the address of the CRT entry point _start.
            # Write ErrorEPC with the address of main
              # clear hazards (makes sure write to ErrorPC has completed)

    # Return from exception will now execute code in main
                      # Exit reset exception handler and start execution of _start.
    

/**************************************************************************************/
all_done:
    # If main returns it will return to this point.  Just spin here.
    b       all_done
    nop

END(__cpu_init)

# Inline the code: fill the rest of space between here and the next exception vector address.

#include "init_caches.S"

/**************************************************************************************
    B O O T   E X C E P T I O N   H A N D L E R S (CP0 Status[BEV] = 1)
**************************************************************************************/
/* NOTE: the linker script must insure that this code starts at start + 0x200 so the exception */
/* vectors will be addressed properly. All .org assume this! */
/* TLB refill, 32 bit task. */
.org 0x200                      # TLB refill, 32 bit task.
    sdbbp                       # This has the effect of starting the debugger
    nop

.org 0x280                      # XTLB refill, 64 bit task. start + 0x280
    sdbbp                       # This has the effect of starting the debugger
    nop

#include "init_cp0.S"

.org 0x300                      # Cache error exception. start + 0x300
    sdbbp                       # This has the effect of starting the debugger
    nop

#include "init_tlb.S"


.org 0x380    /* General exception. */
    # display alternating one-zeros on board LEDs
                # Board LEDs display
                # alternating one-zeros
                # Write pattern to LEDs
    eret
    nop

# If you want the above code to fit into 1k flash you will need to leave out the
# code below. This is the code that covers the debug exception which you normally will not get.

.org 0x480 /* EJTAG debug exception (EJTAG Control Register[ProbTrap] == 0.) */
    li      r24_malta_word, LEDS_ADDR     # If post boot then $24 is clobbered.
    mtc0    a0, C0_DESAVE       # DeSave a0
    mfc0    a0, C0_DEPC         # Read DEPC
    sw      a0, 0(r24_malta_word)    # Display lower 16 bits of DEPC if there is not an EJTAG probe.
    mfc0    a0, C0_DESAVE       # Restore a0
1:  b       1b  /* Stay here */
    nop

