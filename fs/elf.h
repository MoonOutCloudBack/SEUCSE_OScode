#ifndef _ELF_H
#define _ELF_H

#include <stdint.h>

#define IS_ELF(hdr) \
  ((hdr).e_ident[0] == 0x7f && (hdr).e_ident[1] == 'E' && \
   (hdr).e_ident[2] == 'L'  && (hdr).e_ident[3] == 'F')

#define IS_ELF32(hdr) (IS_ELF(hdr) && (hdr).e_ident[4] == 1)
#define IS_ELF64(hdr) (IS_ELF(hdr) && (hdr).e_ident[4] == 2)

#define PT_LOAD 1

#define SHT_NOBITS 8


// ELF 文件头
typedef struct {
  uint8_t  e_ident[16];     // 看上面的宏定义 IS_ELF, 大概是用来记录自己文件类型
  uint16_t e_type;          // 文件类型
  uint16_t e_machine;       // CPU 平台属性
  uint32_t e_version;       // 版本信息
  uint32_t e_entry;         // 入口地址
  uint32_t e_phoff;         // 程序头表的文件偏移 (以字节为单位), 如果文件没有程序头表, 则为零
  uint32_t e_shoff;         // 节头表的文件偏移 (以字节为单位), 如果文件没有节头表, 则为零
  uint32_t e_flags;         // 与文件关联的特定于处理器的标志, 标志名称采用 EF_machine_flag 形式
  uint16_t e_ehsize;        // ELF头的大小 (以字节为单位)
  uint16_t e_phentsize;     // 文件的程序头表中某一项的大小 (以字节为单位), 所有项的大小都相同
  uint16_t e_phnum;         // 程序头表中的项数
  uint16_t e_shentsize;     // 节头的大小 (以字节为单位), 节头是节头表中的一项, 所有项的大小都相同
  uint16_t e_shnum;         // 节头表中的项数
  uint16_t e_shstrndx;      // 与节名称字符串表关联的项的节头表索引
} Elf32_Ehdr;


// ELF 段表
typedef struct {
  uint32_t sh_name; 
  uint32_t sh_type; 
  uint32_t sh_flags; 
  uint32_t sh_addr; 
  uint32_t sh_offset; 
  uint32_t sh_size; 
  uint32_t sh_link; 
  uint32_t sh_info; 
  uint32_t sh_addralign; 
  uint32_t sh_entsize;
} Elf32_Shdr;


// ELF 程序头表
typedef struct
{
  // 程序头表的地址和个数, 是通过 Ehdr (文件头) 的 e_phoff (偏移) e_phnum (个数) 得到的. 
  // 接下来, 我们根据这两个参数, 一个一个读头表. 
  // 需要读取该段的 1. 物理地址 2. 偏移 3. 字节数, 然后用 memCpy, 将各个段加载到相应的物理地址中
  // 注意: 如果内存映像字节数 > 文件映像的字节数, 即, 用来存 elf 的内存区域并没有被充满
  // 那么超出的部分内存应该被置为 0, 也就是 elf.c 的 zero padding

  uint32_t p_type;            // 描述段的类型
  uint32_t p_offset;          // 从文件头到该段第一个字节的偏移
  uint32_t p_vaddr;           // 段的第一个字节, 将放在内存的哪个虚拟地址
  uint32_t p_paddr;           // 仅用于与物理地址相关的系统中, 大概是第一个字节被放在的物理地址吧
  uint32_t p_filesz;          // 段在文件映像中占用的字节数, 可为 0
  uint32_t p_memsz;           // 段在内存映像中占用的字节数, 可为 0
  uint32_t p_flags;           // 段相关的标志
  uint32_t p_align;           // 段在文件中 内存中如何对齐
} Elf32_Phdr;


// ELF 符号表
typedef struct
{
  uint32_t st_name;
  uint32_t st_value;
  uint32_t st_size;
  uint8_t  st_info;
  uint8_t  st_other;
  uint16_t st_shndx;
} Elf32_Sym;


// 接下来是 64 位了
typedef struct {
  uint8_t  e_ident[16];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  uint64_t sh_addr;
  uint64_t sh_offset;
  uint64_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint64_t sh_addralign;
  uint64_t sh_entsize;
} Elf64_Shdr;

typedef struct {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
} Elf64_Phdr;

typedef struct {
  uint32_t st_name;
  uint8_t  st_info;
  uint8_t  st_other;
  uint16_t st_shndx;
  uint64_t st_value;
  uint64_t st_size;
} Elf64_Sym;

extern int load_elf(const uint8_t *elf, const uint32_t elf_size);
int load_elf_sd(const uint8_t *elf, const uint32_t elf_size);
uint32_t get_entry(const uint8_t *elf, const uint32_t elf_size);
#endif