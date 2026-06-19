# 10 - Sysfs Driver

Exposes driver state via sysfs instead of a character device.
No /dev/ file needed — just plain files under /sys/kernel/.

## Attributes
- counter — read/write integer value
- message — read/write string value

## Build
make

## Load & Test
insmod sysfs_driver.ko

ls /sys/kernel/sysfs_driver/

cat /sys/kernel/sysfs_driver/counter
echo 42 > /sys/kernel/sysfs_driver/counter
cat /sys/kernel/sysfs_driver/counter

cat /sys/kernel/sysfs_driver/message
echo "Driver dev rocks" > /sys/kernel/sysfs_driver/message

## Unload
rmmod sysfs_driver
