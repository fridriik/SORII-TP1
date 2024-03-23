/*chardev.c*/
#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h> /*Para sprintf()*/
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h> /*Para get_user y put_user*/
#include <asm/errno.h>

/*Prototipos que irian en un archivo .h*/
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "chardev" /*Asi aparece en /proc/devices*/
#define BUF_LEN 80 /*Largo max del mensaje del dispositivo*/

/*Las variables globales se definen como static*/

static int major; /*Num asignado al driver del dispositivo*/

enum{
	CDEV_NOT_USED = 0,
	CDEV_EXCLUSIVE_OPEN = 1,
};

/*Se encuentra el dispositivo en uso? Asi prevenimos que se tenga acceso multiple al mismo*/
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

/*El mensaje que dara el dispositivo cuando se lo pida*/
static char msg[BUF_LEN + 1];

static struct class *cls;

static struct file_operations chardev_fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release,
};

int __init init_module(void) {
	major = register_chrdev(0, DEVICE_NAME, &chardev_fops);
	if (major < 0) {
		pr_alert("Registrar el char device fallo con %d\n", major);
		return major;
	}
	pr_info("Me asignaron el num %d.\n", major);
	cls = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
	pr_info("Dispositivo creado en /dev/%s\n", DEVICE_NAME);
	printk (KERN_INFO "%s registrado\n", DEVICE_NAME);
	return SUCCESS;
}

static void __exit exit_module(void) { 
	device_destroy(cls, MKDEV(major, 0)); 
	class_destroy(cls); 
	/* Desregistra el dispositivo */
	unregister_chrdev(major, DEVICE_NAME);
	printk (KERN_INFO "%s desregistrado\n", DEVICE_NAME);
}

/*Metodos*/

/*Se llaman cuando un proceso trata de abrir el archivo del dispositivo como por ejemplo "sudo cat /dev/chardev"*/
static int device_open(struct inode *inode, struct file *file) { 
	//static int counter = 0; 
	if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN)) 
		return -EBUSY; 
	//sprintf(msg, "El último mensaje que escribiste fue: %s\n", msg); 
	try_module_get(THIS_MODULE); 
	return SUCCESS; 
} 

/*Se llama cuando un proceso cierra el archivo del dispositivo*/ static int device_release(struct inode *inode, struct file *file) { 
	/*Listos para el proximo llamado*/ 
	atomic_set(&already_open, CDEV_NOT_USED); 
	/*Descontamos el uso ya que una vez abierto el archivo no podemos deshacernos del modulo*/ 
	module_put(THIS_MODULE); 
	return SUCCESS; 
} 

/*Se llama cuando un proceso ya abrio el archivo e intenta leer desde ahi
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset) { 
	Number of bytes actually written to the buffer
	int bytes_read = 0; 
	const char *msg_ptr = msg; 
	if (!*(msg_ptr + *offset)) { 
		//we are at the end of message
		*offset = 0; //reset the offset
		return 0; //signify end of file
	} 
	msg_ptr += *offset; //Actually put the data into the buffer 
	while (length && *msg_ptr) { 
		//The buffer is in the user data segment, not the kernel * segment so "*" assignment won't work. We have to use * put_user which copies data from the kernel data segment to * the user data segment 
		put_user(*(msg_ptr++), buffer++); length--;
		bytes_read++; 
	} 
	*offset += bytes_read; //Most read functions return the number of bytes put into the buffer.
	return bytes_read;
	//return simple_read_from_buffer(buffer, length, offset, msg, strlen(msg));
}*/

//Se llama cuando un proceso quiere escribir en el archivo dev echo "tu mensaje" > /dev/dispositivo 
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off) {
	strncpy(msg, buff, len);
    msg[len] = '\0';
    printk(KERN_INFO "Escribiste en el dispositivo: %s\n", msg);
    return len;
}

//Read invertido
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t * offset)
{
    char reversed_msg[100] = {0};
    int msg_len = strlen(msg);
    int i;
    
    if(msg[msg_len - 1] == '\n'){
    	msg_len--;
    }

    // Invertir el mensaje
    for(i = 0; i < msg_len; i++){
        reversed_msg[i] = msg[msg_len - i - 1];
    }
    
    reversed_msg[msg_len] = '\n';

    return simple_read_from_buffer(buffer, length, offset, reversed_msg, msg_len + 1);
}

module_exit(exit_module);
MODULE_LICENSE("GPL");



