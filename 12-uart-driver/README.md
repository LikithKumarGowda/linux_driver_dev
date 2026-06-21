# 12 - UART Driver

## Part 1 - Userspace UART configuration & test

Configures /dev/ttyAMA1 via termios (baud rate, 8N1, raw mode) and
performs a write + timed read. No data is read back since ttyAMA1 has
no physical loopback wiring in QEMU — this confirms write path and
termios configuration work correctly.

PL011 UART hardware support is already provided by the in-kernel driver;
this lesson works at the userspace/termios layer that sits on top of it.

## Build
make

## Run
./uart_test

