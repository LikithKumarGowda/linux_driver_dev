# 11 - I2C Protocol Driver (Register Map)

Virtual I2C device with multiple registers, modeling how real sensor
chips expose configuration, data, and status via register addressing.

## Register Map
| Register | Address | Purpose |
|---|---|---|
| CONFIG | 0x00 | Enable/disable sensor (bit 0) |
| TEMP   | 0x01 | Temperature value |
| STATUS | 0x02 | Status flags (bit 0 = running) |

Writing CONFIG triggers a side-effect on STATUS, mirroring real hardware
behavior (e.g. enabling a sensor sets a "running" status bit).

## Build
make

## Load & Test
insmod i2c_protocol_driver.ko
echo "i2c-proto-dev 0x49" > /sys/bus/i2c/devices/i2c-0/new_device

cat /dev/i2cproto                  # read all registers
echo "0=1" > /dev/i2cproto         # enable sensor (write CONFIG)
cat /dev/i2cproto
echo "1=50" > /dev/i2cproto        # set TEMP register
cat /dev/i2cproto

## Unload
rmmod i2c_protocol_driver
