# SORII-TP1

## Introduction
Exploration of the creation and management of modules in the Linux kernel, from a basic "Hello World" module to a "Char Device Module" for device  operations.

## Hello World Module
Steps to compile and load the module into the kernel, verification via `dmesg | tail`.

## Char Device Module
Creation and management of a Char Device, including the `init_module` and `exit_module` functions, and operations like `device_open` and `device_release`. Use of atomic variables to avoid race conditions. Implementation of `device_write` to write into the kernel and `device_read` to read from the kernel buffer, with usage examples and necessary permissions.
