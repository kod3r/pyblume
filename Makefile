blume_test: test.c blume.h blume.c murmur.h murmur.c
	gcc -Wall -g -O3 test.c murmur.c blume.c -o blume_test -lm
