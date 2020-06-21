obj-m += wq_dynamic.o
wq_dynamic-objs := 0602_waitqueue_dynamic.o

all:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
