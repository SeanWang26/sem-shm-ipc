

libjthk.so:hkmanager.cpp ../jtprintf.c
	g++ -m32 -g -fPIC -Wl,-rpath,./ -Wall -shared -o libjthk.so hkmanager.cpp ../jtprintf.c libhcnetsdk.so libhpr.so libMPCtrl.so libPlayCtrl.so libStreamTransClient.so libSystemTransform.so
	chmod +x *.so
	cp -r *.so ../

clean:
	rm -f libjthk.so
