all:udp1 udp

udp1: udp1.o   
	gcc -o udp1 udp1.o
	sudo ./udp1

udp: udp.o   checksum.o
	gcc -o udp udp.o  checksum.o
	sudo ./udp

udp1.o:udp1.c  
	gcc -c udp1.c  

udp.o:udp.c  checksum.c checksum.h
	gcc -c udp.c   checksum.c

checksum.o:checksum.c checksum.h
	gcc -c checksum.c

clean:
	rm udp1 udp udp1.o udp.o  checksum.o
