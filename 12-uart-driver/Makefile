obj-m := ldisc_driver.o

KDIR := /home/likith/buildroot/output/build/linux-6.18.7
CROSS := arm-linux-gnueabihf-
CC := arm-linux-gnueabihf-gcc

all:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) modules
	$(CC) -o uart_test uart_test.c
	$(CC) -o attach_ldisc attach_ldisc.c

clean:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) clean
	rm -f uart_test attach_ldisc
