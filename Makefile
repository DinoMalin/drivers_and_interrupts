KERNEL = /lib/modules/$(shell uname -r)/build

obj-m += module.o

all:
	$(MAKE) -C $(KERNEL) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL) M=$(PWD) clean

update:
	rm module.c
	wget https://raw.githubusercontent.com/dinomalin/drivers_and_interrupts/refs/heads/master/module.c --no-check-certificate

fresh: update all
