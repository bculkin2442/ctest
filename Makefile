all: ctest

ctest: ctest.o
	ar -rc libctest.a ctest.o

test: testx
	./test

testx: ctest
	cc -L. -I. -o test test.c -lctest

clean:
	rm *.o *.a test

install:
	cp libctest.a /usr/local/lib
	cp ctest.h /usr/local/include

deinstall:
	rm /usr/local/lib/libctest.a
	rm /usr/local/include/ctest.h
