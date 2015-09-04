# CROSS_COMPILE =
# CROSS_COMPILE += -

CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
RM = rm -f

ARCH = -march=native
CFLAGS = -std=c11 -O3 $(ARCH) -minline-all-stringops -Wno-unused-result -D_FILE_OFFSET_BITS=64
# CFLAGS = -std=c11 -O3 $(ARCH) -minline-all-stringops -Wno-unused-result -D_DEBUG
LDFLAGS = -L./mbedtls/library

OBJS = main.o ttsdown.o http.o utils.o
LIBS = -lmbedcrypto
INCLUDE = -I./mbedtls/include

ifndef WINDOWS
	PROG = HLSDownload.exe
else
	PROG = HLSDownload
endif

$(PROG):$(OBJS) libmbedcrypto.a
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)
	$(STRIP) -s $(PROG)

libmbedcrypto.a:
	$(MAKE) -C ./mbedtls/library

%.o:%.c
	$(CC) $(INCLUDE) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(PROG) *.stackdump
	$(MAKE) -C ./mbedtls/library clean
