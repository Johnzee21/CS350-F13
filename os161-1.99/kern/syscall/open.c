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
open(const char *filename, int flags){
	int result;   //for Erro handling purpose.
	struct vnode *v;//runprogram.c style.
	int ret; //return value.
	struct f_descrip *FD; //FD.
	char *f_name = NULL;
	if(flags!=O_RDONLY && flags!=O_WRONLY && flags!=O_RDWR){
		//if it's non-of the three flags
		//But there are actually more flags.
		//I have no idea if we need to include.
		errno=EINVAL;
		return -1;
	}

	if (filename==NULL){
		errno=EFAULT;
		return -1; //name equals to null, invalid ptr.
	}
     //even a monkey can understand this line.
	if (curproc->f_table->num_files>=OPEN_MAX){
	 errno= EMFILE;
	 return -1;
	}

	f_name = kstrdup(filename);
    result= vfs_open(f_name,flags,0,&v);
    kfree(f_name);
    if (result){
    	return result;
    }
    if (v==NULL){
    	errno=ENOMEN
    	return -1;
    }
     FD = f_descrip_create(filename,v,flags);
    //find a place in the table.
     ret=f_talbe_add(curproc->proc_f_table,FD);
     if (ret <=2){
     	return -1;
     }
     return FD;
}

