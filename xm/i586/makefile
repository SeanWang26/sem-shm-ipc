

libjtxm.so:../xmmanager.cpp ../../jtprintf.c
	g++ -m32 -g -fPIC -Wl,-rpath,./ -Wall -shared -o libjtxm.so ../xmmanager.cpp ../../jtprintf.c libxmnetsdk.so
	chmod +x *.so
	cp -r *.so ../../i586

testd:
	g++ -m32 -g -fPIC -Wl,-rpath,./ -Wall -o  testd ../xmtest.cpp ../../jtprintf.c libxmnetsdk.so -lpthread -lrt

clean:
	rm -f libjtxm.so
