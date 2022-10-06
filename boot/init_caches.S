/*
 * init_caches.S
 *
 *  Created on: Jan 12, 2011
 *  Author: MIPS TECHNOLOGIES, INC
 *  Common Cache initialization
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

#define _BOOTCODE 1

#include "boot.h"
#include <mips/regdef.h>
#include <mips/asm.h>
#include <mips/m32c0.h>

	.set	noreorder           // Don't allow the assembler to reorder instructions.
	.set	noat                // Don't allow the assembler to use r1(at) for synthetic instr.

/**************************************************************************************
**************************************************************************************/

LEAF(init_icache)

	# Can be skipped if using magic simulation cache flush

	# Determine how big the I$ is
			# read C0_Config1

	# Isolate I$ Line Size
		   # extract IL

	# Skip ahead if No I$
	
	nop


				# Now have true I$ line size in bytes

			    # extract IS
	
			    # I$ Sets per way

	# Config1IA == I$ Assoc - 1
			# extract IA
	

			# Total number of sets

	        # Get a KSeg0 address for cacheops

	# Clear TagLo/TagHi registers
			# write C0_ITagLo
			# write C0_ITagHi

	

next_icache_tag:
	# Index Store Tag Cache Op
	# Will invalidate the tag entry, clear the lock bit, and clear the LRF bit
				    # ICIndexStTag
					# Decrement set counter
					# Get next line address

done_icache:

    jr      ra
    nop
END(init_icache)

/**************************************************************************************
* init_dcache invalidates all data cache entries
**************************************************************************************/

LEAF(init_dcache)

			# read C0_Config1

	# Isolate D$ Line Size
			# extract DL
	
	# Skip ahead if No D$
	
	nop

	
				# Now have true D$ line size in bytes

			    # extract DS
	
		        # D$ Sets per way

	# Config1DA == D$ Assoc - 1
			# extract DA
	
			# Get total number of sets

	        # Get a KSeg0 address for cacheops

	# Clear TagLo/TagHi registers
	       # write C0_TagLo
		   # write C0_TagHi
		   # write C0_DTagLo
		   # write C0_DTagHi

	move	a3, a0

next_dcache_tag:
	# Index Store Tag Cache Op
	# Will invalidate the tag entry, clear the lock bit, and clear the LRF bit

    			    # DCIndexStTag
				    # Decrement set counter

			    	# Get next line address

done_dcache:
    jr      ra
    nop
END(init_dcache)

LEAF(change_k0_cca)
	# NOTE! This code must be executed in KSEG1 (not KSGE0 uncached)
    # Set CCA for kseg0 to cacheable
	         	# read C0_Config
				# CCA for single-core processors
                # insert K0
	            # write C0_Config
	jr.hb   ra
    	nop

END(change_k0_cca)
