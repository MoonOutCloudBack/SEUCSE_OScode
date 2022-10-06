#include <inc/tlbop.h>
#include <inc/m32c0.h>
#include <inc/printf.h>
#include <inc/mmu.h>
#include <inc/pmap.h>
extern Pde *boot_pgdir;
extern u32 mCONTEXT;
void print_tlb()
{
    u32 tlb_size = mips_tlb_size();
    printf("TLB size: %d\n", tlb_size);
    unsigned int i;
    tlbhi_t phi;
    tlblo_t plo0, plo1;
    unsigned pmsk;
    for (i = 0; i < tlb_size; ++i)
    {
        mips_tlbri2(&phi, &plo0, &plo1, &pmsk, i);
        printf("%d hi %x lo0 %x lo1 %x pmsk %x\n", i, phi, plo0, plo1, pmsk);
    }
}

