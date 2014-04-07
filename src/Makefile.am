irvm_CFLAGS = -pedantic -Wall -Werror -std=gnu99 
AM_YFLAGS = -d
AM_LFLAGS = -i
bin_PROGRAMS = irvm
irvm_SOURCES = irvm.c irvm.h \
			   consts.h \
			   context.c context.h \
			   decorate.c decorate.h \
			   destroy.c destroy.h \
			   error.c error.h \
			   execute.c execute.h \
			   id.c id.h \
			   mem.c mem.h \
			   primitives.c primitives.h \
			   irparser.y irlexer.l

irvm_LDADD = libccan.a

CLEANFILES=irlexer.c irparser.c irparser.h

noinst_LIBRARIES = libccan.a
libccan_a_SOURCES = \
	ccan/build_assert/build_assert.h \
	ccan/list/list.h \
	ccan/list/list.c \
	ccan/check_type/check_type.h \
	ccan/container_of/container_of.h \
	ccan/cast/cast.h \
	ccan/str/str.h \
	ccan/str/debug.c \
	ccan/str/str_debug.h \
	ccan/str/str.c \
	ccan/typesafe_cb/typesafe_cb.h \
	ccan/htable/htable.c \
	ccan/htable/htable.h \
	ccan/htable/htable_type.h \
	ccan/opt/parse.c \
	ccan/opt/opt.c \
	ccan/opt/private.h \
	ccan/opt/opt.h \
	ccan/opt/usage.c \
	ccan/opt/helpers.c \
	ccan/compiler/compiler.h \
	ccan/hash/hash.c \
	ccan/hash/hash.h
