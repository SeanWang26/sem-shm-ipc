

libjtdh.so:../dhmanager.cpp ../../jtprintf.c
	g++ -m32 -g -fPIC -Wall -shared -Wl,-rpath,./ -o libjtdh.so ../dhmanager.cpp ../../jtprintf.c libdhconfigsdk.so libdhdvr.so libdhnetsdk.so
	chmod +x *.so
	cp *.so ../../i586

clean:
	rm -f libjtdh.so

