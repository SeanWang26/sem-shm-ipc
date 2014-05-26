CC = gcc -ggdb3 

CCC = g++ -ggdb3 

all : libjfront.so frontplug.exe plugtest.exe

libjfront.so:../usersdk.cpp ../frontdevice.cpp ../seansinglebuf.c ../usersdk.cpp
	$(CCC) -m32 -g -fPIC -Wl,-rpath,./ -Wall -shared -o libjfront.so ../usersdk.cpp ../frontdevice.cpp ../seansinglebuf.c ../jtprintf.c -L./ -ldl

frontplug.exe:../main.c ../commnvr.h ../commnvr.cpp ../TimeOutCheck.cpp ../posixsem.cpp ../SemSemIpcUtility.cpp ../jtprintf.c
	$(CCC) -m32 -g -Wall -Wl,-rpath,./ -o frontplug.exe ../main.c ../jtprintf.c ../commnvr.cpp ../TimeOutCheck.cpp ../posixsem.cpp ../SemSemIpcUtility.cpp libjfront.so -lpthread -lrt
	cp frontplug.exe zhamponxm_SDK.exe

plugtest.exe: ../plugtest.cpp ../jtsimplestackback.c
	$(CCC) -m32  -g -Wall -Wl,-rpath,./ -o plugtest.exe ../plugtest.cpp ../jtsimplestackback.c -L./ -ljfront -ljtxm -ljthk -ljtdh -lxmnetsdk -lrt -lpthread

.PHONY : all

clean:
	rm -f *.out *.o *.exe 
	rm -f libjfront.so *.exe