#include <types.h>
#include <lib.h>
#include <array.h>
#include <vnode.h>
#include <limits.h>
#include <ftable.h>
#include <synch.h>


struct f_descrip *
f_descrip_create(const char *name,struct vnode* v_n,int o_flag){

    struct f_descrip *FD;
    
    FD=kmalloc(sizeof(struct f_descrip));
    if (FD==NULL){
        kfree(FD);
        return NULL;
    }
    FD->name = kstrdup(name);
    //Erro check
    if (FD->name ==NULL){
        kfree(FD);
    }
    FD->open_flag = o_flag;//close;
    FD->f_offset = (off_t)0; //cast, initialize as 0;
    FD->v_node = v_n; //Vnode;
    //erro check.
    if (FD->v_node == NULL){
        kfree(FD->name);
        kfree(FD);
        return NULL;
    }
    FD->f_lock=lock_create("file lock");
    return FD;
}


void f_descrip_destroy(struct f_descrip *fd){
    KASSERT(fd !=NULL);
    lock_destroy(fd->f_lock);
    kfree(fd->name);
    kfree(fd);
    //Have to do something with Vnode here;
    //
    //
    //
}

struct f_table *f_table_create(void){
    struct f_table *FT;
    //    struct f_descrip *stdin;
    //    struct f_descrip *stdout;
    //    struct f_descrip *stderr;
    FT= kmalloc(sizeof(struct f_table));
    //erro check
    if (FT==NULL){
        kfree(FT);
        return NULL;
    }
    FT->num_files=0; //0 files opened, should be 3;(console);
    FT->f_array=array_create();
    FT->e_table=array_create(); 
    KASSERT(FT->f_array!=NULL);//ensure.
    KASSERT(FT->e_table!=NULL);//ensure.
    //reserve 0,1,2 to put stdin stderr stdout.
    //
    return FT;
}


void f_table_destroy(struct f_table *ft){
    for (unsigned i=0;i<array_num(ft->f_array);i++){
        //descript
        f_descrip_destroy(array_get(ft->f_array,i));
    }
    array_destroy(ft->f_array);
    //I am not sure if we need to clean up this array as well.
    array_destroy(ft->e_table);
    kfree(ft);
}

//A function write to find NULL slot, return 0 if no empty slot.
// int f_table_empty_slot(struct f_table *ft){
//     for (int i=3;i<=f_table->Cur_Num;i++){//starting from the 3rd position.
//         if (f_table_get(ft,i)==NULL){
//             return i;
//         }
//     }
//     return 0;
// }
//
//Changed the f_table_add to return the index.
int f_table_add(struct f_table *ft,struct f_descrip *fd){
    // int null_index;
    // null_index=f_table_empty_slot(ft);
    /* Check if there any NULL element in the table.
       if so , fill up the NULL pos first.
        if not, add new FD at end of the table.
        NOTE:
        I didnt use array_remove here cuz that will make f_id invalid.
        */
    // if (null_index==0){
    //     int ret;
    //     ret=array_add(ft->f_array,fd);
    //     if(ret){ //erro check
    //         KASSERT("array add failed!");
    //     }
    //     ft->Cur_Num++;
    // }
    // else {
    //     array_set(ft->f_array,null_index,fd);
    // }
        int ret;
    if (array_num(ft->e_table)==0){
        ret=array_add(ft->f_array,fd,NULL);
        int index = array_num(ft->f_array)-1;
        if(ret){
            KASSERT("array add failed in f_table_add");
            return -1;
        }
        if (index >2){
            return index;
        }
    }
    else{
        unsigned *null_index;
        //we use a fifo scheme here.
        null_index=array_get(ft->e_table,0);
        if(*null_index<=2){
            KASSERT("f_table_add failed: got an < 2 number from e_table!");
        }
        array_set(ft->f_array,*null_index,fd);
        ft->num_files++; //increment number of files in the table.
        //Here's where fifo happens. we always remove 0 first.
        array_remove(ft->e_table,0);
        return null_index;
    }
}

//file table getter function. return a FD.
struct f_descrip *f_table_get(const struct f_table *ft,unsigned index){
    return array_get(ft->f_array,index);
}

//Delete a element and set it as NULL.
int f_table_delete(struct f_table *ft,unsigned index){
    unsigned *index_ptr;
    if (index<=2){
        //try to delete stdin/out/erro.
        return 1;
    }
    struct f_descrip *fd=f_table_get(ft,index);
    f_descrip_destroy(fd);
    array_set(ft->f_array,index,NULL);
    *index_ptr = index;
    ft->num_files--; //decrement files.
    //we add the empty slot number to this array everytime, so that we don't need to walk through the table to find empty slot everytime.
    array_add(ft->e_table,index_ptr,NULL);
    return 0;
}

/* Initialization of stdin,stdout,stderr.
*/
int console_init(struct f_table *ft){
    struct f_descrip *stdin;
    struct f_descrip *stdout;
    struct f_descrip *stderr;
    struct vnode *in_vn;
    struct vnode *out_vn;
    struct vnode *err_vn;
    int stdin_result;
    int stdout_result;
    int stderr_result;
    char *console =NULL;
    console=kstrdup("con:");
    stdin_result=vfs_open(console,O_RDONLY,&in_vn);
    stdout_result=vfs_open(console,O_WRONLY,&out_vn);
    stderr_result=vfs_open(console,O_WRONLY,&err_vn);
    if(stdin_result){
        return stdin_result;
    }
    if (stdout_result){
        return stdout_result;
    }
    if (stderr_result){
        return stderr_result;
    }
    char *input;
    char *output;
    char *error;
    input=kstrdup("stdin");
    output=kstrdup("stdout");
    error=kstrdup("stderr");
    stdin=f_descrip_create(input,in_vn,O_RDONLY);
    stdout=f_descrip_create(out,out_vn,O_WRONLY);
    stderr=f_descrip_create(error,err_vn,O_WRONLY);
    f_table_add(ft,stderr);
    f_table_add(ft,stdout);
    f_table_add(ft,stdin);     //note the order here.
    kfree(console);
    kfree(input);
    kfree(output);
    kfree(error);
    return 0;
}
















