PKGDIR          ?= ../..
L4DIR           ?= $(PKGDIR)/../..

include $(L4DIR)/mk/Makeconf

#MODE           = shared
MODE            = static
TARGET          = logging-client

SRC_CC          = client.cc
CXXFLAGS        += -Wall -Wreorder

REQUIRES_LIBS    = cxx_libc_io cxx_io libpthread libstdc++ libc_be_mem

include $(L4DIR)/mk/prog.mk

