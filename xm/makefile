

libjtxm.so:xmmanager.cpp ../jtprintf.c
	g++ -m32 -fPIC -Wl,-rpath,./ -Wall -shared -o libjtxm.so xmmanager.cpp ../jtprintf.c libxmnetsdk.so
	chmod +x *.so
	cp -r *.so ../

clean:
	rm -f libjtxm.so
