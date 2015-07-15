
obj-m += xssocket.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -Wall -o xssocket_sender xssocket_sender.c
	gcc -Wall -o xssocket_receiver xssocket_receiver.c
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f xssocket_sender xssocket_receiver *.o *~
install:
	install -d $(DESTDIR)/lib/modules/$(shell uname -r)/kernel/drivers/
	install -d $(DESTDIR)/usr/local/bin/
	install -m0755 -p xssocket.ko  $(DESTDIR)/lib/modules/$(shell uname -r)/kernel/drivers/
	install -m0755 -p xssocket_sender   $(DESTDIR)/usr/local/bin/
	install -m0755 -p xssocket_receiver   $(DESTDIR)/usr/local/bin/

