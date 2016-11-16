pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libKeyFrame.pc

libKeyFrameincludedir = $(includedir)
libKeyFrameinclude_HEADERS = libKeyFrame.h

lib_LTLIBRARIES = libKeyFrame.la
libKeyFrame_la_SOURCES 	= libKeyFrame.c	\
						  TinyPngOut.c
libKeyFrame_la_CFLAGS 	= ${FFMPEG_CFLAGS}
libKeyFrame_la_LIBADD	= ${FFMPEG_LIBS}

bin_PROGRAMS = keyframes

keyframes_SOURCES	= keyframes.c
keyframes_CFLAGS 	= ${FFMPEG_CFLAGS}
keyframes_LDADD		= libKeyFrame.la

noinst_PROGRAMS = test
test_SOURCES	= test.c
test_CFLAGS 	= ${FFMPEG_CFLAGS}
test_LDADD 		= libKeyFrame.la
