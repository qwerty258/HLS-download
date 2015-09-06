# CROSS_COMPILE =
# CROSS_COMPILE += -

CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)gcc
STRIP := $(CROSS_COMPILE)strip
RM := rm -rf

CFLAGS := -std=c11 -Os -minline-all-stringops -Wno-unused-result -D_FILE_OFFSET_BITS=64
# CFLAGS = -std=c11 -Os -minline-all-stringops -Wno-unused-result -D_FILE_OFFSET_BITS=64 -DHAVE_DEBUG -DHAVE_MKV_PACK
LDFLAGS :=

OBJS := main.o ttsdown.o http.o utils.o aes.o
LIBS :=
INCLUDEDIR :=

ifndef WINDOWS
	PROG := HLSDownload.exe
else
	PROG := HLSDownload
endif

$(PROG):$(OBJS)
	$(LD) $(LDFLAGS) -o $(PROG) $(OBJS) $(LIBS)
	$(STRIP) -s $(PROG)

%.o:%.c
	$(CC) $(INCLUDEDIR) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJS) $(PROG) *.stackdump
