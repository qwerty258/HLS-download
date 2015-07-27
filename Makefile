# CROSS_COMPILE =

CC = $(CROSS_COMPILE)-gcc
LD = $(CROSS_COMPILE)-gcc
STRIP = $(CROSS_COMPILE)-strip

RM = rm -f

ARCH = -march=core2
CFLAGS = -std=c11 -O3 $(ARCH) -minline-all-stringops -Wno-unused-result
# CFLAGS = -std=c11 -O3 $(ARCH) -minline-all-stringops -Wno-unused-result -D_DEBUG
LDFLAGS =

OBJS = main.o ttsdown.o http.o utils.o
LIBS = -lcrypto
PROG = ttsdownload

$(PROG):$(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)
	$(STRIP) -s $(PROG)

%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(PROG) *.stackdump
