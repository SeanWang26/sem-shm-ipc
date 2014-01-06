CC = gcc -ggdb3 

CCC = g++ -ggdb3 

all : frontplug.out frontclient.out

frontplug.out:main.c
	$(CC) -Wall -o frontplug.out main.c shm.c sem.c -lpthread -lrt

frontclient.out:clientmain.c shm.c shm.h sem.c sem.h
	$(CC) -Wall -o frontclient.out clientmain.c shm.c sem.c -lpthread -lrt

.PHONY : all


clean:
	rm -f *.out *.o 
