# SORII-TP1

## Introduction
Exploration of the creation and management of modules in the Linux kernel, from a basic "Hello World" module to a "Char Device Module" for device  operations.

## Hello World Module
Steps to compile and load the module into the kernel.
1. Place the source code file of the module along with the Makefile in the same directory.
2. Open a terminal in that directory and run the command `make` to compile the kernel module.
3. Load the module with the command `sudo insmod module.ko`.
4. Verify the module load with the command `dmesg | tail`, as long as we have a print in the init function.
5. Unload the module with the command `sudo rmmod module`.
6. Verify the module unload with the command `dmesg | tail`, as long as we have a print in the exit function.
7. Use the command `make clean` to clean the files generated by the compilation process.

## Char Device Module
Creation and management of a Char Device, including the `init_module` and `exit_module` functions, and operations like `device_open` and `device_release`. Use of atomic variables to avoid race conditions. Implementation of `device_write` to write into the kernel and `device_read` to read from the kernel buffer, with usage examples and necessary permissions.
