obj-m := ioctl_driver.o

KDIR := /home/likith/buildroot/output/build/linux-6.18.7
CROSS := arm-linux-gnueabihf-
TEST_CC := arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) modules
	$(TEST_CC) -o test_ioctl test_ioctl.c

clean:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) clean
	rm -f test_ioctl
