# 02 - Character Device Driver

A character device driver that creates /dev/chardev.
Userspace can write data to it and read it back.

## Build
make

## Load & Test
insmod chardev.ko
echo "Hello from userspace" > /dev/chardev
cat /dev/chardev
dmesg | tail

## Unload
rmmod chardev
