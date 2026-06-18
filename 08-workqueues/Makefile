obj-m := workq_driver.o

KDIR := /home/likith/buildroot/output/build/linux-6.18.7
CROSS := arm-linux-gnueabihf-

all:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) modules

clean:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) clean
