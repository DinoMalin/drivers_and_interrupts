KERNEL = /lib/modules/$(shell uname -r)/build

obj-m += module.o

all:
        ${MAKE} -C ${KERNEL} M=$(PWD) modules

clean:
        ${MAKE} -C ${KERNEL} M=$(PWD) clean
