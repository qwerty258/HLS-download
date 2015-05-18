CROSS_COMPILE =

CC = $(CROSS_COMPILE)-gcc
LD = $(CROSS_COMPILE)-gcc
STRIP = $(CROSS_COMPILE)-strip

ARCH = -msse4.2
CFLAGS = -std=c11 -O3 $(ARCH) -minline-all-stringops -Wno-unused-result -DVERSION=\"1.3.0\"
LDFLAGS =

OBJS = main.o http.o utils.o
LIBS = -lcrypto
PROG = ttsdownload

$(PROG):$(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)
	$(STRIP) -s $(PROG)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -vf $(OBJS) $(PROG) *.stackdump
