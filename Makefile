all: ctest

ctest: ctest.o
	ar -rc libctest.a ctest.o

test: testx
	./test

testx: ctest
	cc -L. -I. -o test test.c -lctest

clean:
	rm *.o *.a test
