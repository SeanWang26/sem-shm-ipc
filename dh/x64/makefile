

libjtdh.so:../dhmanager.cpp ../../jtprintf.c
	g++ -g -fPIC -Wall -shared -Wl,-rpath,./ -o libjtdh.so ../dhmanager.cpp ../../jtprintf.c libdhconfigsdk.so libdhdvr.so libdhnetsdk.so
	chmod +x *.so
	cp *.so ../../x64

clean:
	rm -f libjtdh.so

