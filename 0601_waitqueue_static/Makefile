obj-m += wq_static.o
wq_static-objs := 0601_waitqueue_static.o

all:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
