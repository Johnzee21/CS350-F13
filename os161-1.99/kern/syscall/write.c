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
#include <uio.h>
int
write(int fd, const void *buf, size_t nbytes){
  	struct iovec iov;
  	struct uio u_io;
  	int result;
    struct f_descrip *FD;
    /*
    	NEED MASSIVE ERROR CHECK HERE!
    */
    FD=f_table_get(curproc->proc_f_table,fd); //get fd from file table.
    //get a offset from FD.
    off_t pos=FD->f_offset;
    //init uio.
  	uio_kinit(&iov,&u_io, buf, nbytes ,pos ,UIO_WRITE);
  	//write the buffer into result.
    result=VOP_WRITE(FD->v_node,&u_io);

    //do some basic error check.
    if (result){
    	return -1;
    }

     //refer to uio.h.
    //Here's 2 ways to implement this.
    // FD->f_offset+=(nbytes-uio.uio_resid);
    FD->f_offset=u.uio_offset;
    return 0;
}


//....................