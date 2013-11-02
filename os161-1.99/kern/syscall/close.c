#include <types.h>
#include <kern/errno.h>
#include <kern/syscall.h>
#include <lib.h>
#include <mips/trapframe.h>
#include <thread.h>
#include <current.h>
#include <syscall.h>
#include <opt-A2.h>
#include <kern/unistd.h>
#include <ftable.h>
#include <unistd.h>
#include <errno.h>

extern int errno;
int
close(int fd){
	struct f_descrip *f_table_get(const struct f_table *ft,unsigned index);
	struct f_descrip *f_des;
	if (fd<0){
		errno=EBADF;
		return -1;
	}
	f_des=f_table_get(curproc->proc_f_table,fd);
	vfs_close(f_des->v_node);
	//might need to decrement reference count here.
    ftable_delete(curproc->proc_f_table,fd);
    return 0;
}
