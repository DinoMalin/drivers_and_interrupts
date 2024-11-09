KERNEL = /lib/modules/$(shell uname -r)/build
obj-m += dinologger.o
TARGET_MODULE = dinologger

all:
	$(MAKE) -C $(KERNEL) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL) M=$(PWD) clean
load:
	sudo insmod ./$(TARGET_MODULE).ko
unload:
	sudo rmmod $(TARGET_MODULE)
reload: unload load
