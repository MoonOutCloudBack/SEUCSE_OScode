#pragma once

struct Trapframe;

// Activate the kernel shell,
// optionally providing a trap frame indicating the current state
// (NULL if none).
void shell(struct Trapframe *tf);

// Functions implementing shell commands.
int mon_help(int argc, char **argv, struct Trapframe *tf);
int mon_kerninfo(int argc, char **argv, struct Trapframe *tf);
int mon_play(int argc, char **argv, struct Trapframe *tf);
int mon_game(int argc, char **argv, struct Trapframe *tf);
int mon_tlb(int argc, char **argv, struct Trapframe *tf);
int mon_ls(int argc, char **argv, struct Trapframe *tf);
int mon_touch(int argc, char **argv, struct Trapframe *tf);
int mon_mkdir(int argc, char **argv, struct Trapframe *tf);
int mon_rm(int argc, char **argv, struct Trapframe *tf);
int mon_about(int argc, char **argv, struct Trapframe *tf);
int mon_2048(int args, char * argc[]);
int mon_debug(int args, char * argc[]);
int mon_cd(int argc, char **argv, struct Trapframe *tf);
int mon_read(int argc, char **argv, struct Trapframe *tf);
int mon_write(int argc, char **argv, struct Trapframe *tf);
char* Int2String(int num,char *str);
int test_banker();
int run(char *buf, struct Trapframe *tf);
char * trim (const char *str);
int redirect(char*cmd, struct Trapframe *tf);