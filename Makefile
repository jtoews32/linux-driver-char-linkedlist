KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := llddrvr.o

all: clean run 
	@make -s clean

run: load PipeWriterApp PipeWaiterApp
	echo "echo input" > /dev/llddrvr;
	echo "echo input" > /dev/llddrvr;
	./PipeWriterApp;
	./PipeWaiterApp;
	dd if=/proc/sequence count=1;

load: llddrvr.o
	-su -c "insmod ./llddrvr.ko; mknod -m 666 /dev/llddrvr c 33 0;"
#	-su -c "insmod ./CDD3.ko; mknod -m 666 /dev/CDD3 c 33 10;"
	ls -l /dev/llddrvr
	grep llddrvr /proc/devices

llddrvr.o:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

###
###  Alternatively, you may want to use the early 2.6 syntax of
###  $(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
###

PipeWriterApp:
	-gcc -o PipeWriterApp PipeWriterApp.c;
	
PipeWaiterApp:
	-gcc -o PipeWaiterApp PipeWaiterApp.c;	

unload:
	-su -c " rmmod llddrvr; \
	rm -fr /dev/llddrvr;"
#	-su -c " rmmod CDD3; \
#	rm -fr /dev/CDD3;"

clean: unload
	-@rm -fr *.o llddrvr*.o llddrvr*.ko .llddrvr*.* llddrvr*.*.* PipeWriterApp PipeWaiterApp .tmp_versions [mM]odule*

