# Linux Driver Development on BeagleBone (QEMU)

Learning Linux device driver development using QEMU ARM emulator.
No physical hardware required.

## Environment
- Host: Windows + WSL2
- Emulator: QEMU (vexpress-a9 ARM)
- Buildroot: ARM Linux image
- Cross-compiler: arm-linux-gnueabihf-gcc

## Lessons Completed

1. **Hello World Module** — basic kernel module with init/exit
2. **Character Device Driver** — read/write via /dev/chardev
3. **IOCTL** — custom commands (clear, get_len, set_msg)
4. **Platform Driver** — device tree bindings, probe/remove
5. **GPIO Driver** — simulated virtual GPIO pins
6. **Interrupt/Timer Handling** — kernel timers firing every 2 seconds
7. **I2C Driver** — virtual temperature sensor over I2C
8. **Kernel Timers & Workqueues** — interrupt context vs process context
9. **DMA** — concepts only (not implemented, QEMU vexpress hardware limitation)
10. **Sysfs Driver** — exposing driver state via /sys/kernel/ attributes
