obj-m += procfs_read.o
procfs_read-objs := 0301_procfs_read.o

all:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
