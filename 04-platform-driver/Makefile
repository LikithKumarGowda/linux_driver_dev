obj-m := platform_driver.o

KDIR := /home/likith/buildroot/output/build/linux-6.18.7
CROSS := arm-linux-gnueabihf-
DTC := dtc

all:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) modules
	$(DTC) -@ -I dts -O dtb -o mydevice.dtbo mydevice.dts

clean:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) clean
	rm -f mydevice.dtbo
