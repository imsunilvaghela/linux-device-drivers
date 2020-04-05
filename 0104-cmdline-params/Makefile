obj-m += 0104_cmdline_param.o

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) modules 

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean
