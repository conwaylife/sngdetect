CC = gcc
CFLAGS = -O3

all:	sngdetect

sngdetect:	sngdetect.o sngdetect2.o sngdetect3.o
	$(CC) -o sngdetect sngdetect.o sngdetect2.o sngdetect3.o

clean:
	rm *.o sngdetect
