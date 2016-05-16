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

MODULE_LICENSE("GPL");

#define LLD		"llddrvr"
#define LLDMAJOR	33
#define LLDMINOR	0
#define LLDNUMDEVS	6
#define CMD0 0
#define CMD1 1
#define CMD2 2

struct t_struct {
	int counter;
    char *storage;
};

static struct t_struct t_file_info;

static struct j_node {
	char *data;
	int id;
	int len;
	struct list_head list;

};

static struct j_node global_list;
static struct proc_dir_entry* j_file;

unsigned int counter = 0;
int memory_counter = 0;
unsigned int memory_buffer[4096];


struct cdev cdev;
dev_t  devno;
char t_buf[4096];
char *t_storage=t_buf;

struct completion comp;
DECLARE_COMPLETION(comp);


struct j_node *create_j_node(int id, char *data, int len)
{
    struct j_node *node = (struct j_node *) kmalloc(sizeof(struct j_node), GFP_KERNEL);
    node->id = id;
    node->data = data;
    node->len = len;

    memory_buffer[memory_counter++] = node;
    return node;
}













static int j_show(struct seq_file *m, void *v)
{
	struct list_head *pos, *q;
	struct j_node *tmp;
	int i = 0;
	int j = 0;

	list_for_each_safe(pos, q, &global_list.list)
	{
		tmp= list_entry(pos, struct j_node, list);
		seq_printf(m, "%d\n %s \n", i, tmp->data);

		i++;
	}

	seq_printf(m, "used memory allocated %i\n", memory_counter);

	for(j=0; j<memory_counter; j++) {
		seq_printf(m, "%d ", memory_buffer[j]);
	}

	seq_printf(m, " \n" );

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

static ssize_t t_read (struct file *file, char *buf, size_t count, loff_t *ppos)
{
	struct list_head *pos, *q;
	struct j_node *tmp;
	int len, err, i=0;
    struct t_struct *file_info_local = &t_file_info;


    printk(KERN_INFO "REQUEST TO READ ppos=%i   \n",   *ppos  );


	list_for_each_safe(pos, q, &global_list.list)
	{
		tmp= list_entry(pos, struct j_node, list);
		printk(KERN_INFO "READ %i-of-%d found: \"%s\" %i \n",i, file_info_local->counter, tmp->data, tmp->len);
		len =  tmp->len;

		if(i == *ppos) {
			break;
		}
		i++;
	}

	err = copy_to_user(buf, tmp->data, len);

	printk(KERN_INFO "READ \"%s\" count=%i  ppos=%i %i counter=%i len=%i \n", buf, count, *ppos, ppos, counter, len);

	*ppos = i;

	if (err != 0)
		return -EFAULT;

	counter = 0;
	++file_info_local->counter;

	return len;
}



static ssize_t t_write (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int err, i = 0;
	struct j_node *tmp;

	err = copy_from_user(t_storage,buf,count);
	if (err != 0)
		return -EFAULT;

	printk(KERN_INFO "writing %s", buf);




	tmp= create_j_node(10 , NULL, strlen(buf) );
	tmp->data = (char*) vmalloc( strlen(buf) + 1 );

	memcpy( tmp->data, buf, strlen(buf) + 1 );



	memory_buffer[memory_counter++] = tmp->data ;


	list_add_tail(&(tmp->list), &(global_list.list));

	complete(&comp);
	counter += count;
	return count;
}


static long t_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	printk(KERN_ALERT "ioct %i\n", cmd);

	switch ( cmd ) {
		case CMD0:
			printk(KERN_ALERT "ioct CMD0 %i\n", cmd);
			break;
		case CMD1:
			printk(KERN_ALERT "ioctl CMD1 %i\n", cmd);
			break;
		case 4:
			printk(KERN_ALERT "WAITING ON COMPETION\n" );
			wait_for_completion(&comp);
			printk(KERN_ALERT "COMPLETION DONE");
		default:
			printk(KERN_ALERT "ioctl CMD1 %i\n", cmd);
		//	retval = -EINVAL;
	}

	return retval;
}


static loff_t t_llseek(struct file *file, loff_t newpos, int whence) {

	int pos;
	struct t_struct *file_info_local = &t_file_info;

	switch (whence) {
	case SEEK_SET:        // CDDoffset can be 0 or +ve
		pos = newpos;
		printk(KERN_ALERT "llseek SEEK_SET %i\n", pos);
		break;
	case SEEK_CUR:        // CDDoffset can be 0 or +ve
		pos = (file->f_pos + newpos);
		printk(KERN_ALERT "llseek SEEK_SET %i\n", pos);
		// pos=(file->f_pos + newpos);
		//
		break;
	case SEEK_END:        // CDDoffset can be 0 or +ve

		// pos=(thisCDD->counter + newpos);
		// count list and say where the end is

		printk(KERN_ALERT "llseek SEEK_SET %i\n", 20);

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
    int x,i;

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


	tmp= create_j_node(i, "1-LOAD", strlen( "1-LOAD" ));

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

		printk(KERN_INFO "CLEANUP \"%s\"", tmp->data);
		vfree(tmp->data);
		list_del(pos);
		kfree(tmp);
	}

	remove_proc_entry("sequence", NULL);
	printk(KERN_ALERT "DONE" );
}

module_init(t_init);
module_exit(t_exit);
