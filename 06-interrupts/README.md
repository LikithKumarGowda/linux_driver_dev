# 06 - Interrupt/Timer Driver

Driver that uses kernel timers to simulate hardware interrupts.
Timer fires every 2 seconds and increments a counter.

## Commands
- Read counter: cat /dev/timerdev
- Stop timer:   echo "stop" > /dev/timerdev
- Start timer:  echo "start" > /dev/timerdev
- Reset counter: echo "reset" > /dev/timerdev

## Build
make

## Load & Test
insmod timer_driver.ko
cat /dev/timerdev
dmesg | tail

## Unload
rmmod timer_driver
