BIN = bin
OBJ = obj

.PHONY: all clean

all: vlantaggerd

clean:
	sudo rm libvlan.a /usr/bin/vlantaggerd /etc/systemd/system/vlantaggerd.service /var/run/vlantaggerd.pid /var/run/filter.txt /var/run/iface.txt -R $(OBJ)

vlan.o: VirtualLan/vlan.c
	mkdir $(OBJ)
	gcc -c -o $(OBJ)/vlan.o VirtualLan/vlan.c

filter.o: VirtualLan/filter.c
	gcc -c -o $(OBJ)/filter.o VirtualLan/filter.c

libvlan.a: vlan.o filter.o
	ar cr libvlan.a $(OBJ)/vlan.o $(OBJ)/filter.o

vlantaggerd.o: vlantaggerd.c libvlan.a
	gcc -c -o $(OBJ)/vlantaggerd.o vlantaggerd.c

vlantaggerd: vlantaggerd.o
	sudo gcc -o /usr/bin/vlantaggerd $(OBJ)/vlantaggerd.o -L. -lvlan