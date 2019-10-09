ifneq ($(KERNELRELEASE),) 
   obj-m := etx_int_11.o
else 

KERNELDIR ?= /lib/modules/$(shell uname -r)/build 

PWD := $(shell pwd)

default: 
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules  
endif 
clean:
	$(MAKE) -C $(KERNELDIR)  M=$(PWD) clean
