PKGDIR		?= ..
L4DIR		?= $(PKGDIR)/../..

vpath %.bmp ..

MODE		= static
#MODE		= shared
TARGET		= pong-client
SRC_CC		= main.cc
REQUIRES_LIBS   = cxx_libc_io cxx_io libpthread libstdc++ libc_be_mem

include $(L4DIR)/mk/prog.mk
