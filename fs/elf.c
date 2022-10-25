#include "elf.h"
#include <stddef.h>
#include "..\inc\printf.h"

static void *memSet(void *s, int c, size_t n)
{
  if (NULL == s || n < 0)
    return NULL;
  char * tmpS = (char *)s;
  while(n-- > 0)
    *tmpS++ = c;
    return s; 
}

  int memCpy(void *dest, void *src, uint32_t n)
  {
    if (NULL == dest || NULL == src || n < 0)
      return 0;
    uint32_t *tempDest = (uint32_t *)dest;
    uint32_t *tempSrc = (uint32_t *)src;
    uint32_t i =0;
    for(i = 0; i <= n / 4; i++)        // remain n % 4 memory not set, so i <= n / 4
    {
      tempDest[i] = tempSrc[i];
    }

    int x=99;
    x++;
    return x;
  }

int load_elf(const uint8_t *elf, const uint32_t elf_size) {
  // sanity checks
  if(elf_size <= sizeof(ELF32_Ehdr))
    return 1;                   /* too small */

  const ELF32_Ehdr *eh = (const ELF32_Ehdr *) elf;
  if(!IS_ELF32(*eh))
    return 2;                   /* not a elf32 file */

  const ELF32_Ehdr *ph = (const ELF32_Ehdr *)(elf + eh->e_phoff);
  if(elf_size < eh->e_phoff + eh->e_phnum*sizeof(*ph))
    return 3;                   /* internal damaged */

  uint32_t i;
  for(i=0; i<eh->e_phnum; i++) {
    if(ph[i].p_type == PT_LOAD && ph[i].p_memsz) { /* need to load this physical section */
      
      
      if(ph[i].p_filesz) {                         /* has data */
        

      }
      if(ph[i].p_memsz > ph[i].p_filesz) {         /* zero padding */
        

      }
    }
  }
  return 0;
}

int load_elf_sd(const uint8_t *elf, const uint32_t elf_size) {
  
  // sanity checks
  if(elf_size <= sizeof(ELF32_Ehdr))
    return 1;                   /* too small */

  const ELF32_Ehdr *eh = (const ELF32_Ehdr *) elf;
  if(!IS_ELF32(*eh))
    return 2;                   /* not a elf32 file */

  const ELF32_Ehdr *ph = (const ELF32_Ehdr *)(elf + eh->e_phoff);
  if(elf_size < eh->e_phoff + eh->e_phnum*sizeof(*ph))
    return 3;                   /* internal damaged */

  uint32_t i;
  for(i=0; i<eh->e_phnum; i++) {
    if(ph[i].p_type == PT_LOAD && ph[i].p_memsz) {     /* need to load this physical section */ 
      if(ph[i].p_filesz) {                         /* has data */
       
       
                                                   /* internal damaged */
        
      }
      if(ph[i].p_memsz > ph[i].p_filesz) {         /* zero padding */
        

      }
    }
  }
  return 0;
}

uint32_t get_entry(const uint8_t *elf, const uint32_t elf_size)
{
    const Elf32_Ehdr *eh = (const Elf32_Ehdr *)elf;
    const Elf32_Phdr *ph = (const Elf32_Phdr *)(elf + eh->e_phoff);  
    return  eh->e_entry;
}

