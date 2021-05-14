# $FreeBSD$

.include <src.opts.mk>
.include <src.lua.mk>

SHLIB_NAME=	dialog_wrap.so
SHLIBDIR=	${LIBDIR}/flua

CFLAGS+= \
	-I${SRCTOP}/contrib/lua/src \
	-I${SRCTOP}/lib/liblua \

CFLAGS+= -I${SRCTOP}/contrib/dialog -DHAVE_DIALOG
LIBADD= dialog ncursesw

SRCS+= dialog_wrap.c

.include <bsd.lib.mk>
