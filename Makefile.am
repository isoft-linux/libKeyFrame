libKeyFrameincludedir = $(includedir)
libKeyFrameinclude_HEADERS = libKeyFrame.h

lib_LTLIBRARIES = libKeyFrame.la
libKeyFrame_la_SOURCES 	= libKeyFrame.c	\
						  TinyPngOut.c
libKeyFrame_la_CFLAGS 	= ${FFMPEG_CFLAGS}
libKeyFrame_la_LIBADD	= ${FFMPEG_LIBS}

noinst_PROGRAMS	= test

test_SOURCES	= test.c
test_CFLAGS 	= ${FFMPEG_CFLAGS}
test_LDADD 		= libKeyFrame.la
