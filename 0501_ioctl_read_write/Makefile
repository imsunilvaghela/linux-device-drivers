obj-m += ioctl_rw_custom.o
ioctl_rw_custom-objs := 0501_ioctl_read_write.o

all:
	make -C /lib/modules/`uname -r`/build M=`pwd` modules
clean:
	make -C /lib/modules/`uname -r`/build M=`pwd` clean
