

libjtxm.so:xmmanager.cpp ../jtprintf.c
	g++ -g -fPIC -Wl,-rpath,./ -Wall -shared -o libjtxm.so xmmanager.cpp ../jtprintf.c libxmnetsdk.so
	chmod +x *.so
	cp -r *.so ../../x64

clean:
	rm -f libjtxm.so
