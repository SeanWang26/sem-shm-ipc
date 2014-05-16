CC = gcc -ggdb3 

CCC = g++ -ggdb3 

all : libjfront.so frontplug.exe plugtest.exe

# frontplug.out frontclient.out xmtest.out


libjfront.so:usersdk.cpp frontdevice.cpp
	$(CCC) -m32 -g -fPIC -Wl,-rpath,./ -Wall -shared -o libjfront.so usersdk.cpp frontdevice.cpp jtprintf.c -L./ -ldl

frontplug.exe:main.c commnvr.h commnvr.cpp TimeOutCheck.cpp posixsem.cpp jtprintf.c
	$(CCC) -m32 -g -Wall -Wl,-rpath,./ -o frontplug.exe main.c jtprintf.c commnvr.cpp TimeOutCheck.cpp  posixsem.cpp libjfront.so -lpthread -lrt
	cp frontplug.exe zhamponxm_SDK.exe

plugtest.exe:plugtest.cpp
	$(CCC) -m32  -g -Wall -Wl,-rpath,./ -o plugtest.exe plugtest.cpp -L./ -ljfront -ljtxm -lxmnetsdk -lrt -lpthread

#frontplug.out:main.c  shm.c sem.c vsem.c usersdk.cpp frontdevice.cpp dhmanager.cpp commnvr.h commnvr.cpp TimeOutCheck.cpp posixsem.cpp jtprintf.c
#	$(CCC) -g -Wall -o dahua.out jtprintf.c commnvr.cpp TimeOutCheck.cpp main.c posixsem.cpp shm.c sem.c vsem.c usersdk.cpp frontdevice.cpp dhmanager.cpp showstruct.cpp ./libdhnetsdk.so -lpthread -lrt
#	cp dahua.out zhdahua_SDK.exe

#frontclient.out:clientmain.cpp shm.c shm.h sem.c sem.h vsem.c jtprintf.c
#	$(CCC)  -Wall -o frontclient.out jtprintf.c showstruct.cpp clientmain.cpp shm.c sem.c vsem.c  -lpthread -lrt

#xmtest.out:test.cpp
#	g++ -m32 -o xmtest.out test.cpp ./libxmnetsdk.so


.PHONY : all


clean:
	rm -f *.out *.o *.exe 
	rm -f libjfront.so *.exe