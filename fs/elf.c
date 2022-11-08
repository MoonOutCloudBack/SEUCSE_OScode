#include "elf.h"
#include <stddef.h>
#include "..\inc\printf.h"

/*
load elf 的逻辑:
- 先得到 elf 头表, 然后索引 elf 头, 得到每个程序段的头表
- 然后 for 循环 读每个程序段的头表, 根据记录的 程序段长度、内存地址、分配给它的内存的长度 等等信息, 挨个 load

指导手册中说的 1. 把 elf 先读到内存缓冲区 + 2. 调用 load_elf() 的代码，
在 env/env.c 的 load_elf_mapper() 函数里
*/


static void *memSet(void *s, int c, size_t n)
{
  // 参数: set 目的地地址, 要 set 成的整数 (貌似只取末 8 位?), set 多少个字节
  if (NULL == s || n < 0)
    return NULL;
  char * tmpS = (char *)s;
  while(n-- > 0)
    *tmpS++ = c;
    return s; 
}

int memCpy(void *dest, void *src, uint32_t n)
{
  // 参数: copy 目的地地址, copy 源地址, copy 多少个字节
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

  // 无论如何, elf 文件也得有头表 (ELF32_Ehdr) 这么大吧
  if(elf_size <= sizeof(ELF32_Ehdr))
    return 1;                   /* too small */

  // 得到 ELF 头表
  const ELF32_Ehdr *eh = (const ELF32_Ehdr *) elf; 
  // 判一下是否 ELF, 定义在 elf.h
  if(!IS_ELF32(*eh))
    return 2;                   /* not a elf32 file */

  // 得到程序头表的起始地址, e_phoff 是程序头表偏移
  const ELF32_Phdr *ph = (const ELF32_Phdr *)(elf + eh->e_phoff); 
  // 得到的 elf 文件, 怎么还没有 (据推测) 所有程序头表加起来那么大
  if(elf_size < eh->e_phoff + eh->e_phnum*sizeof(*ph)) 
    return 3;                   /* internal damaged */

  uint32_t i;
  // 现在我们一个一个 load 程序段
  for(i=0; i<eh->e_phnum; i++) {
    // p_memsz: 段在内存映像中占用的字节数, 可为 0
    if(ph[i].p_type == PT_LOAD && ph[i].p_memsz) { /* need to load this physical section */
      // p_filesz: 段在文件映像中占用的字节数, 可为 0
      // p_paddr: 该段第一个字节, 被放在哪个物理地址
      printf("[load_elf] still alive ... writing %d bytes to %x \n\r", 
              ph[i].p_filesz, (uint32_t)ph[i].p_paddr);
      if(ph[i].p_filesz) {                         /* has data, 这个段不为空,需要 load 一下 */
        // elf 文件中, 这个段是不完整的
        // p_offset: 从文件头 (最开始) 到该段第一个字节的偏移
        if(elf_size < ph[i].p_offset + ph[i].p_filesz)
          return 3;             /* internal damaged */
        memCpy((uint8_t *)ph[i].p_paddr, elf + ph[i].p_offset, ph[i].p_filesz);
      }
      if(ph[i].p_memsz > ph[i].p_filesz) {         /* zero padding */
        memSet((uint8_t *)ph[i].p_paddr + ph[i].p_filesz, 0, ph[i].p_memsz - ph[i].p_filesz);
      }
    }
  }
  return 0;
}

int load_elf_sd(const uint8_t *elf, const uint32_t elf_size) {
  // 貌似跟上面 load_elf 一模一样
  
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
    if(ph[i].p_type == PT_LOAD && ph[i].p_memsz) {      /* need to load this physical section */ 
      printf("[load_elf] still alive ... writing %d bytes to %x \n\r",
              ph[i].p_filesz, (uint32_t)ph[i].p_paddr);
      if(ph[i].p_filesz) {                              /* has data */
        if(elf_size < ph[i].p_offset + ph[i].p_filesz)
          return 3;                                     /* internal damaged */
        memCpy((uint8_t *)ph[i].p_paddr, elf + ph[i].p_offset, ph[i].p_filesz);
      }
      if(ph[i].p_memsz > ph[i].p_filesz) {              /* zero padding */
        memSet((uint8_t *)ph[i].p_paddr + ph[i].p_filesz, 0, ph[i].p_memsz - ph[i].p_filesz);
      }
  }
  return 0;
}

uint32_t get_entry(const uint8_t *elf, const uint32_t elf_size)
{
    // eh->e_entry: 记载在 elf 头表里的, 程序的入口地址
    const Elf32_Ehdr *eh = (const Elf32_Ehdr *)elf;
    const Elf32_Phdr *ph = (const Elf32_Phdr *)(elf + eh->e_phoff);  
    return eh->e_entry;
}

