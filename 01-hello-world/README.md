# 01 - Hello World Kernel Module

First Linux kernel module. Prints a message on load and unload.

## Build
make

## Load
insmod hello.ko
dmesg | tail

## Unload
rmmod hello
dmesg | tail
