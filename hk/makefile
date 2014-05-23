
libjthk.so:hkmanager.cpp ../jtprintf.c ../seansinglebuf.c
	g++ -m32 -g -fPIC -Wl,-rpath,./ -Wall -shared -o libjthk.so hkmanager.cpp ../jtprintf.c ../seansinglebuf.c libhcnetsdk.so
	chmod +x *.so
	cp -r *.so ../

testd:
	g++ -m32 -g -fPIC -Wl,-rpath,./ -Wall -o  testd testd.cpp  hkmanager.cpp ../jtprintf.c libjthk.so libhcnetsdk.so


clean:
	rm -f libjthk.so
