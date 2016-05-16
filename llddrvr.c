#include <linux/version.h>
#include <generated/utsrelease.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/memory.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/completion.h>



#include "llddrvr.h"

MODULE_LICENSE("GPL");


struct t_struct {
	int counter;
    char *storage;
};

static struct j_node {
	char *data;
	int id;
	int len;
	int entry;
	struct list_head list;

};

static struct t_struct t_file_info;
static struct j_node global_list;
static struct proc_dir_entry* j_file;

unsigned int counter = 0;
int memory_counter = 0;
unsigned int memory_buffer[4096];
struct cdev cdev;
dev_t  devno;

char t_buf[30];
char *t_storage=t_buf;

static char *string;
static int data;


struct completion comp;
DECLARE_COMPLETION(comp);

struct j_node *create_j_node(int id, char *data, int len) {
	struct t_struct *file_info_local = &t_file_info;
	struct j_node *node = (struct j_node *) kmalloc(sizeof(struct j_node),
			GFP_KERNEL);

	node->id = id;
	node->data = data;
	node->len = len;
	node->entry = ++file_info_local->counter;

	memory_buffer[memory_counter++] = node;
	return node;
}

static int j_show(struct seq_file *m, void *v)
{
	struct list_head *pos, *q;
	struct j_node *tmp;
	int i = 0, j = 0;

	seq_printf(m, "llddrvr: contains following linked list entries\n\n" );

	list_for_each_safe(pos, q, &global_list.list)
	{
		tmp = list_entry(pos, struct j_node, list);
		seq_printf(m, "%d:\n    %i %s \n\0", i, tmp->entry, tmp->data);

		i++;
	}

	seq_printf(m, "\n\nMemory allocations: %i\n\n\0", memory_counter);

	for(j=0; j<memory_counter; j++) {
		seq_printf(m, "%d ", memory_buffer[j]);
	}

	seq_printf(m, "\n\n\n\0" );

	return 0;
}

static int j_open(struct inode *inode, struct file *file)
{
     return single_open(file, j_show, NULL);
}

static const struct file_operations j_fops = {
     .owner	= THIS_MODULE,
     .open	= j_open,
     .read	= seq_read,
     .llseek	= seq_lseek,
     .release	= single_release,
};


static int t_open (struct inode *inode, struct file *file)
{
	return 0;
}

static int t_release (struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t t_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct j_node *tmp;
	int err;

	printk(KERN_INFO "Write called with ppos=%i , count=%i", count, *ppos);

	err = copy_from_user(t_storage,buf,count);

	printk(KERN_INFO "Write copied to %s claims length of ", t_storage, strlen(t_storage));


	if (err != 0)
		return -EFAULT;

	tmp= create_j_node(1, NULL, strlen(t_storage) );
	tmp->data = (char*) vmalloc( strlen(t_storage) + 1 );

	memcpy( tmp->data, t_storage, strlen(t_storage) + 1 );
	memory_buffer[memory_counter++] = tmp->data ;

	list_add_tail(&(tmp->list), &(global_list.list));

	complete(&comp);
	counter += count;

	printk(KERN_INFO "Write called with %s return count=%i", t_storage, count);

	return count;
}




static ssize_t t_read (struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct list_head *pos, *q;
	struct j_node *tmp;
	int len, err, i=0;
    struct t_struct *file_info_local = &t_file_info;
    int entry = 	*ppos;

    printk(KERN_INFO "Read ppos=%i   \n",   *ppos  );

	list_for_each_safe(pos, q, &global_list.list)
	{
		tmp = list_entry(pos, struct j_node, list);

		printk(KERN_INFO "Searching for %d - %i-of-%d found:%s %d \n",
				entry,
				i,
				file_info_local->counter,
				tmp->data,
				tmp->len);

		len = tmp->len;

		if(i == *ppos) {
			i++;
			break;
		}

		i++;
	}

	err = copy_to_user(buf, tmp->data, len);
	if (err != 0)
		return -EFAULT;



	*ppos = i;
	printk(KERN_INFO "Reed reset ppos \"%i\"  ", i );
	printk(KERN_INFO "Read reset buf  \"%s\"  ", buf );

	return len;
}






static long t_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	printk(KERN_ALERT "ioctl %i %i", cmd , arg);
	int retval = 0;

	switch ( cmd ) {





	case 1:
		printk(KERN_ALERT "ioctl write");
		// for "writing" data to arg from userspace
		// if (!access_ok(VERIFY_WRITE, (void __user *) arg, sizeof(int)))
			// return -EFAULT;

		// use copy_from_user for pointer references
		//if (copy_from_user(&data, (int *) arg, sizeof(int)))
		//		return -EFAULT;
		// otherwise, use straightforward reference.

		printk("w. data==%ld\n",(unsigned long) arg);
		retval = data = (unsigned long) arg;
		break;

	case 2:// for reading data from arg into userspace


		if (!access_ok(VERIFY_READ, (void __user *) arg, sizeof(int))) {
			printk(KERN_ALERT "access bump");
			return -EFAULT;
		}

		// use copy_to_user for pointer references
		if (copy_to_user((int *) arg, &data, sizeof(int))) {

			printk(KERN_ALERT "ioctl bail");
			return -EFAULT;
		}

		printk(KERN_ALERT "ioctl read");
		printk("r. data==%ld\n",(unsigned long) data);
		retval = data;
		break;



	default:
		printk(KERN_ALERT "ioctl bail");
		retval = -EINVAL;
	}
	return retval;


	/*
	int retval = 0;
	printk(KERN_ALERT "ioct %i\n", cmd);

	switch ( cmd ) {
		case READ:
			printk(KERN_ALERT "ioct CMD0 %i\n", cmd);
			break;
		case WRITE:
			printk(KERN_ALERT "ioctl CMD1 %i\n", cmd);
			break;
		case COMPLETE:
			printk(KERN_ALERT "WAITING ON COMPETION\n" );
			wait_for_completion(&comp);
			printk(KERN_ALERT "COMPLETION DONE");
		default:
			printk(KERN_ALERT "ioctl CMD1 %i\n", cmd);
		//	retval = -EINVAL;
	}

	return retval;
	*/
}


static loff_t t_llseek(struct file *file, loff_t newpos, int whence) {

	int pos, i = 0;
	struct t_struct *file_info_local = &t_file_info;
	struct list_head *posp, *q;


	list_for_each_safe(posp, q, &global_list.list)
	{
		i++;
	}

	switch (whence) {
	case SEEK_SET:        // CDDoffset can be 0 or +ve
		pos = newpos;
		printk(KERN_INFO "Lseek reset ppos %i on SEEK_SET of %i\n", pos, i);
		break;
	case SEEK_CUR:        // CDDoffset can be 0 or +ve
		pos = (file->f_pos + newpos);
		printk(KERN_INFO "Lseek reset ppos %i on SEEK_CUR of %i\n", pos, i);
		// pos=(file->f_pos + newpos);
		//
		break;
	case SEEK_END:        // CDDoffset can be 0 or +ve

		pos=i-1;

		printk(KERN_INFO "Lseek reset ppos %i on SEEK_END of %i\n", pos,i);

		break;
	default:
		return -EINVAL;
	}

	/*
	 if ((pos < 0)||(pos>thisCDD->counter))
	 return -EINVAL;
	 */

	file->f_pos = pos;
	return pos;
}

static const struct file_operations t_fops = {
		owner: THIS_MODULE, 	// struct module *owner
		open: t_open, 	// open method
		read: t_read,	// read method
		write: t_write, 	// write method
		release: t_release, 	// release method
		unlocked_ioctl:  t_ioctl, 	// ioctl method
		llseek:  t_llseek  // covered in detail in Ch6. Just for fwd ref now.
};

static int __init t_init(void) {

	struct proc_dir_entry *entry;
    struct j_node *tmp;
    int i;

    struct t_struct *file_info_local = &t_file_info;
    file_info_local->counter = 0;
	file_info_local->storage = NULL;


	devno = MKDEV(LLDMAJOR, LLDMINOR);
	i = register_chrdev_region(devno, LLDNUMDEVS, LLD);

	printk(KERN_ALERT "INIT" );


	if (i < 0) {
		printk(KERN_ALERT "Error (%i) adding LLD", i);
		return i;
	}

	cdev_init(&cdev, &t_fops);

	cdev.owner = THIS_MODULE;
	cdev.ops = &t_fops;
	i = cdev_add(&cdev, devno, LLDNUMDEVS);

	if (i) {
		printk(KERN_ALERT "Error (%d) adding LLD", i);
		return i;
	}

	j_file = proc_create("sequence", 0, NULL, &j_fops);

	if (!j_file) {
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&global_list.list);


	tmp= create_j_node(file_info_local->counter , "initialization", strlen( "initialization" ));

	list_add_tail(&(tmp->list), &(global_list.list));

	return 0;
}

static void __exit t_exit(void) {
	struct list_head *pos, *q;
	struct j_node *tmp;


	cdev_del(&cdev);
	unregister_chrdev_region(devno, LLDNUMDEVS);
	printk(KERN_ALERT "EXIT" );

	if (list_empty(&global_list)) {
		remove_proc_entry("sequence", NULL);
		return;
	}

	list_for_each_safe(pos, q, &global_list.list) {

		tmp= list_entry(pos, struct j_node, list);

		printk(KERN_INFO "Remove \"%s\"", tmp->data);
		vfree(tmp->data);
		list_del(pos);
		kfree(tmp);
	}

	remove_proc_entry("sequence", NULL);
	printk(KERN_ALERT "DONE" );
}

module_init(t_init);
module_exit(t_exit);
