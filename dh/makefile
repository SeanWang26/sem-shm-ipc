

libjtdh.so:dhmanager.cpp ../jtprintf.c
	g++ -m32 -fPIC -Wall -shared -Wl,-rpath,./ -o libjtdh.so dhmanager.cpp ../jtprintf.c libdhconfigsdk.so libdhdvr.so libdhnetsdk.so
	chmod +x *.so
	cp *.so ../

clean:
	rm -f libjtdh.so

