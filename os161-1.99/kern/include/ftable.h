
/* wrote by Ken Yundi Huang OCT-23
   file table and file descriptor
   version 1
*/

#include <types.h>
#include <array.h>
#include <vnode.h>
#include <limits.h>

//Set maximun file name length as 20.


/* our file descriptor structure.
    name: the name of file.
    f_offset: file offset.
    open_flag: shows the file open status.
    v_node:the vnode pointer here.
    lock: the lock we used to protect the file.
 */
struct f_descrip {
    char* name;
    volatile off_t f_offset;
    int open_flag;
    struct  vnode* v_node;
    struct lock* f_lock;
};


/* our file table structure.
   It is indeed an array provided by os161 lib.
   
    f_array:           it is an array of file descriptor.
    e_table:  i use this to keep track of empty slot for efficiency purpose.
 */
struct f_table{
    unsigned num_files;
    struct array* f_array;
    struct array* e_table;
};

/* creat a f_descript, take an name and a vnode.
 */
struct f_descrip *f_descrip_create(const char *name,struct vnode* v_n);
/* destroy a v_node, but I don't know if we need to clean the vnode.
 */
void f_descrip_destroy(struct f_descrip *fd);

/*ftable stuff*/
struct f_descrip *f_table_get(const struct f_table *ft,unsigned index);

void console_init(struct f_table *ft);
struct f_table *f_table_create(void);

void f_table_add(struct f_table *ft,struct f_descrip *fd);

void f_table_destroy(struct f_table *ft);

int f_table_delete(struct f_table *ft,unsigned index);
