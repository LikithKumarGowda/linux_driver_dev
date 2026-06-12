# 04 - Platform Driver & Device Tree

Driver that uses device tree bindings. Kernel automatically
calls probe() when it finds a matching compatible string.

## How it works
1. Device tree node has compatible = "likith,mydevice"
2. Kernel matches it to platform_driver
3. probe() is called automatically on boot

## Build
make

## Load & Test
insmod platform_driver.ko
dmesg | tail

## Unload
rmmod platform_driver
