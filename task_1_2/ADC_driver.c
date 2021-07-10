#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "HolyDoly" );
MODULE_DESCRIPTION( "My module" );
MODULE_SUPPORTED_DEVICE( "ADC" );

#define SUCCESS 0
#define DEVICE_NAME "ADC"

static int device_open( struct inode *, struct file * );
static int device_release( struct inode *, struct file * );
static ssize_t device_read( struct file *, char *, size_t, loff_t * );

static int major_number;
static int is_device_open = 0;
ssize_t number = 0;

static struct file_operations fops =
 {
  .read = device_read,
  .open = device_open,
  .release = device_release
 };

int __init init_module( void )
{
 printk( KERN_ALERT "ADC driver loaded!\n" );
 major_number = register_chrdev(0, DEVICE_NAME, &fops );

 if ( major_number < 0 )
 {
  printk( "Registering the character device failed with %d\n", major_number );
  return major_number;
 }

 printk( "ADC module is loaded!\n" );
 return SUCCESS;
}

void __exit cleanup_module(void) {
    unregister_chrdev( major_number, DEVICE_NAME );
    printk( KERN_ALERT "Test module is unloaded!\n" );
}

static int device_open( struct inode *inode, struct file *file )
{
 if ( is_device_open )
  return -EBUSY;
 is_device_open++;
 return SUCCESS;
}

static int device_release( struct inode *inode, struct file *file )
{
 is_device_open--;
 return SUCCESS;
}

static ssize_t device_read( struct file *filp, char *buffer, size_t length, loff_t *offset )
{
    char *snum = kmalloc(32, GFP_KERNEL);
    char *ptr = snum;
    sprintf(snum, "%ld%c", number, '\0');
    while (*ptr && length) 
    {
        put_user(*(ptr++), buffer++);
        length--;
    }
    number++;
    kfree(snum);
    return SUCCESS;
}