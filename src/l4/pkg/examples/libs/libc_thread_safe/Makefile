PKGDIR		?= ../..
L4DIR		?= $(PKGDIR)/../..

TARGET		= uclibc_thread_safe
SRC_CC		= main.cc
REQUIRES_LIBS   = libpthread
REQUIRES_LIBS  += libc_be_mem
DEPENDS_PKGS    = $(REQUIRES_LIBS)

include $(L4DIR)/mk/prog.mk
