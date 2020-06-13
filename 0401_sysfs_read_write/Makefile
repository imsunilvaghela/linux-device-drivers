obj-m += sysfs_rw.o
sysfs_rw-objs := 0401_sysfs_read_write.o

all:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
