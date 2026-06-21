# 12 - UART Driver

## Part 1 - Userspace UART configuration & test (uart_test.c)

Configures /dev/ttyAMA1 via termios (baud rate, 8N1, raw mode) and
performs a write + timed read. No data is read back since ttyAMA1 has
no physical loopback wiring in QEMU — this confirms the write path and
termios configuration work correctly. PL011 UART hardware support is
already provided by the in-kernel driver; this works at the
userspace/termios layer on top of it.

## Part 2 - Kernel-side line discipline driver (ldisc_driver.c)

Registers a custom tty line discipline (N_MYPROTO) and attaches it to
/dev/ttyAMA1 using a raw TIOCSETD ioctl (attach_ldisc.c) since
ldattach isn't available in this minimal Buildroot image.

This is the same mechanism real protocols use to sit on top of UART:
- PPP frames network packets over a serial line
- Bluetooth HCI-UART frames Bluetooth packets
- LIN bus frames bytes into LIN protocol frames

### Limitation
The receive_buf() callback fires on data arriving on the RX line, not
on writes from userspace (TX). Without physical loopback wiring (or a
QEMU pty-backed serial connection), no inbound bytes reach the ldisc
to demonstrate receive_buf() firing live. Registration, attachment,
and the correct hook structure are confirmed working — seeing
receive_buf() trigger is a good milestone for real BeagleBone hardware
with an actual wired UART peripheral.

## Build
make

## Run
./uart_test
insmod ldisc_driver.ko
./attach_ldisc /dev/ttyAMA1

## Unload
rmmod ldisc_driver
