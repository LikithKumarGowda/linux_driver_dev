# 08 - Kernel Timers & Workqueues

Demonstrates the separation between interrupt context (timer)
and process context (workqueue).

## Key concept
- Timer callback: runs in interrupt context, must be fast, cannot sleep
- Workqueue: runs in process context, can sleep, can do heavy work

## Flow
1. Timer fires every 2s (interrupt context)
2. Timer schedules work via schedule_work()
3. Workqueue handler runs later (process context)
4. Workqueue can safely call msleep() and other blocking calls

## Load & Test
insmod workq_driver.ko
cat /dev/workqdev
dmesg | tail

## Unload
rmmod workq_driver
