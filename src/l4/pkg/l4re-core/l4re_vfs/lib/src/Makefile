PKGDIR    ?= ../..
L4DIR     ?= $(PKGDIR)/../../..

PC_FILENAME = libl4re-vfs
TARGET	    = libl4re-vfs.o.a
SRC_CC	    = vfs.cc
REQUIRES_LIBS = l4re-util

include $(L4DIR)/mk/lib.mk

CFLAGS   += -fvisibility=hidden
CXXFLAGS += -fvisibility=hidden
# No exception information as unwinder code might use malloc and friends
CXXFLAGS += -fno-exceptions -fno-rtti
LDFLAGS        += --wrap=malloc --wrap=free --wrap=realloc
