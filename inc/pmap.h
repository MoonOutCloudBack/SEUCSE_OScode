#ifndef _PMAP_H_
#define _PMAP_H_

#include "types.h"
#include "queue.h"
#include <mmu.h>
#include <printf.h>


// 空闲内存链表 维护，管理剩余的物理内存 进程申请内存的时候，分配给它一页（4kB）
// 在 MIPS CPU 中，地址转换以 4KB 大小为单位，称为页. 我们使用 Page 结构体
// 来作为记录一页内存的相关信息的数据结构
struct Page {
    // pp_link 是当前节点指向链表中下一个节点的指针，其类型为 LIST_ENTRY(Page)
    struct Page* pp_link;      /* free list link */

    // pp_ref 用来记录这一物理页面的引用次数
    u_short pp_ref;
};

extern struct Page *pages;

void set_physic_mm();
void vm_init();
void mips_init();
void page_init(void);
void page_check();
int page_alloc(struct Page **pp);
void page_free(struct Page *pp);
void page_decref(struct Page *pp);
int pgdir_walk(Pde *pgdir, u_long va, int create, Pte **ppte);
int page_insert(Pde *pgdir, struct Page *pp, u_long va, u_int perm);
struct Page *page_lookup(Pde *pgdir, u_long va, Pte **ppte);
void page_remove(Pde *pgdir, u_long va) ;
void tlb_invalidate(Pde *pgdir, u_long va);
void tlb_out(u_int entryhi);
void boot_map_segment(Pde *pgdir, u_long va, u_long size, u_long pa, int perm);
u_long page2ppn(struct Page *pp);
u_long page2pa(struct Page *pp);
u_long page2kva(struct Page *pp);
u_long va2pa(Pde *pgdir, u_long va);
struct Page *pa2page(u_long pa);

#endif /* _PMAP_H_ */