recover:	main.o ext2.o
	gcc -g main.o ext2.o -o recover
main.o: main.c
	gcc -g -c main.c 
ext2.o: ext2.c ext2.h
	gcc -g -c ext2.c
clean:
	-rm *.o
.PHONY: clean

