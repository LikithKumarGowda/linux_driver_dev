# 03 - IOCTL Driver

Driver that accepts commands from userspace via ioctl.

## Commands
| Command        | Description                  |
|----------------|------------------------------|
| IOCTL_CLEAR    | Clears the kernel buffer     |
| IOCTL_GET_LEN  | Returns buffer length        |
| IOCTL_SET_MSG  | Sets message via ioctl       |

## Build
make

## Load & Test
insmod ioctl_driver.ko
./test_ioctl

## Unload
rmmod ioctl_driver
