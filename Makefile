CC	= gcc
CFLAGS	= -Wall -O2  -I/opt/local/include
LDFLAGS	= -L/opt/local/lib -lgmp

all: endianess.h secshare

secshare: secshare.o encrypt.o decrypt.o rijndael.o sha1.o share.o common.o
	$(CC) $(LDFLAGS) -o $@ $^

endianess.h: endianess
	./endianess

endianess: endianess.o
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -rf *.o secshare endianess *~ *.bak endianess.h
