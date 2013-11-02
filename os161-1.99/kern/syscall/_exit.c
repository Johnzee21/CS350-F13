#include <types.h>
#include <kern/errno.h>
#include <kern/syscall.h>
#include <lib.h>
#include <mips/trapframe.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>
#include <opt-A2.h>
#include <proc.h>

int _exit(int exitcode){
    proc_destroy(curproc);
    return exitcode;
}
