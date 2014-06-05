

libjtsn.so:../snmanager.cpp ../../jtprintf.c ../../seansinglebuf.c
	g++ -g -fPIC -Wl,-rpath,./ -Wall -shared -o2 -o libjtsn.so ../snmanager.cpp ../../jtprintf.c ../../seansinglebuf.c -lcurl
	chmod +x *.so
	cp -r *.so ../../x64

clean:
	rm -f libjtsn.so

