# 05 - Simulated GPIO Driver

A virtual GPIO chip implemented as a character device.

## How it works
- Creates /dev/gpiodev with 8 virtual pins
- Pins default to LOW (0)
- Read all pin states via cat
- Set pins via echo "pin=value"

## Build
make

## Load & Test
insmod gpio_driver.ko
cat /dev/gpiodev
echo "3=1" > /dev/gpiodev
cat /dev/gpiodev

## Unload
rmmod gpio_driver
