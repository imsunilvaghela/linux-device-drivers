obj-m += 0103_module_spanning.o
0103_module_spanning-objs := 0103_module_spanning_init.o 0103_module_spanning_exit.o

all:
	make -C /lib/modules/`uname -r`/build M=$(PWD) modules

clean:
	make -C /lib/modules/`uname -r`/build M=$(PWD) clean
