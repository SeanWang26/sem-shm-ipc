CC = gcc -ggdb3 

CCC = g++ -ggdb3 

all : frontplug.out frontclient.out

frontplug.out:main.c  shm.c sem.c vsem.c
	$(CC) -m32 -Wall -o frontplug.out main.c shm.c sem.c vsem.c -lpthread -lrt

frontclient.out:clientmain.c shm.c shm.h sem.c sem.h vsem.c
	$(CC) -m32 -Wall -o frontclient.out clientmain.c shm.c sem.c vsem.c -lpthread -lrt

.PHONY : all


clean:
	rm -f *.out *.o 
