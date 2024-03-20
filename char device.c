#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> // Para la API de registro de char device. Para la estructura file_operations

int major; // Variable para almacenar el major number asignado al dispositivo

int init_module(void) {
    major = register_chrdev(0, "mi_char_device", &fops); // Registra el char device
    if (major < 0) {
        printk(KERN_ALERT "Registro de char device fallido con %d\n", major);
        return major;
    }
    printk(KERN_INFO "Char device registrado con major number %d\n", major);
    return 0; // Retorno exitoso
}

void cleanup_module(void) {
    unregister_chrdev(major, "mi_char_device"); // Desregistra el char device
    printk(KERN_INFO "Char device desregistrado\n");
}

// Definición de las funciones de operación
static ssize_t read(struct file *file, char __user *user_buffer, size_t count, loff_t *ppos);
static ssize_t write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos);
static int open(struct inode *inode, struct file *file);
static int release(struct inode *inode, struct file *file);

// Definición de la estructura file_operations
static struct file_operations fops = {
    .read = read,
    .write = write,
    .open = open,
    .release = release
};
