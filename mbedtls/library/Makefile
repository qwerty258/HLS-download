
# Also see "include/mbedtls/config.h"

CFLAGS	?= -O2
WARNING_CFLAGS ?=  -Wall -W -Wdeclaration-after-statement
LDFLAGS ?=

LOCAL_CFLAGS = $(WARNING_CFLAGS) -I../include -D_FILE_OFFSET_BITS=64
LOCAL_LDFLAGS =

ifdef DEBUG
LOCAL_CFLAGS += -g3
endif

# MicroBlaze specific options:
# CFLAGS += -mno-xl-soft-mul -mxl-barrel-shift

# To compile on Plan9:
# CFLAGS += -D_BSD_EXTENSION

# if were running on Windows build for Windows
ifdef WINDOWS
WINDOWS_BUILD=1
endif

# To compile as a shared library:
ifdef SHARED
# all code is position-indep with mingw, avoid warning about useless flag
ifndef WINDOWS_BUILD
LOCAL_CFLAGS += -fPIC -fpic
endif
endif

SOEXT_CRYPTO=so.0

DLEXT=so
# OSX shared library extension:
# DLEXT=dylib

# Windows shared library extension:
ifdef WINDOWS_BUILD
DLEXT=dll
endif

OBJS_CRYPTO=	aes.o

.SILENT:

.PHONY: all static shared clean

all: static

static: libmbedcrypto.a

# crypto
libmbedcrypto.a: $(OBJS_CRYPTO)
	echo "  AR    $@"
	$(AR) rc $@ $(OBJS_CRYPTO)
	echo "  RL    $@"
	$(AR) s $@

.c.o:
	echo "  CC    $<"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) -c $<

clean:
	rm -f *.o libmbed*
