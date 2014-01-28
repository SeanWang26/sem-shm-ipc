CC = gcc -ggdb3 

CCC = g++ -ggdb3 

all : frontplug.out frontclient.out xmtest.out

frontplug.out:main.c  shm.c sem.c vsem.c frontdevice.c xmmanager.c commnvr.h commnvr.cpp
	$(CCC) -m32 -g -Wall -o frontplug.out main.c shm.c sem.c vsem.c frontdevice.c xmmanager.c posixsem.cpp commnvr.cpp ./libxmnetsdk.so -lpthread -lrt

frontclient.out:clientmain.c shm.c shm.h sem.c sem.h vsem.c
	$(CC) -m32 -Wall -o frontclient.out clientmain.c shm.c sem.c vsem.c -lpthread -lrt

xmtest.out:test.cpp
	g++ -m32 -o xmtest.out test.cpp ./libxmnetsdk.so


.PHONY : all


clean:
	rm -f *.out *.o 
