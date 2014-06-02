


libjthk.so:../hkmanager.cpp ../../jtprintf.c ../../seansinglebuf.c
	g++ -g -fPIC -Wl,-rpath,./ -Wall -shared -o2 -o libjthk.so ../hkmanager.cpp ../../jtprintf.c ../../seansinglebuf.c libhcnetsdk.so
	chmod +x *.so
	cp -r *.so ../../x64

testd:
	g++ -g -fPIC -Wl,-rpath,./ -Wall -o  testd testd.cpp  ../hkmanager.cpp ../../jtprintf.c libjthk.so libhcnetsdk.so


clean:
	rm -f libjthk.so





































