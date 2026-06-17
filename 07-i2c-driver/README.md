# 07 - I2C Driver

Virtual I2C temperature sensor driver.
Simulates reading/writing temperature via I2C bus.

## How it works
1. Driver registers with I2C subsystem
2. Kernel calls probe() when device is found at 0x48
3. Creates /dev/tempdev for userspace access

## Load & Test
# Instantiate I2C device
echo "virtual-temp-sensor 0x48" > /sys/bus/i2c/devices/i2c-0/new_device

# Load driver
insmod i2c_driver.ko

# Read temperature
cat /dev/tempdev

# Set temperature
echo "37" > /dev/tempdev
cat /dev/tempdev

## Unload
rmmod i2c_driver
