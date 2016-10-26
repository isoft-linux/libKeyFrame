# use pkg-config for getting CFLAGS and LIBS
FFMPEG_LIBS=    libavdevice                        \
                libavformat                        \
                libavfilter                        \
                libavcodec                         \
                libswresample                      \
                libswscale                         \
                libavutil                          \
				libpng							   \

CC		= gcc
CFLAGS	+= -O2 -Wall -fPIC -std=gnu11
CFLAGS  += -g -DDEBUG
CFLAGS	:= $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
CPATH	=
LIBPATH	=
LIBS	+= -lm
LIBS 	:= $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LIBS)

all: libKeyFrame

libKeyFrame:
	$(CC) -o libKeyFrame.o -c $(CFLAGS) $(CPATH) libKeyFrame.c
	$(CC) -o TinyPngOut.o -c $(CFLAGS) $(CPATH) TinyPngOut.c
	$(CC) -o libKeyFrame.so libKeyFrame.o TinyPngOut.o $(LIBPATH) $(LIBS) -shared

test:
	$(CC) -o test.o -c $(CFLAGS) $(CPATH) test.c
	$(CC) -o test test.o libKeyFrame.o $(LIBPATH) $(LIBS) -L. -lKeyFrame

makePNG:
	$(CC) -o makePNG.o -c $(CFLAGS) $(CPATH) makePNG.c
	$(CC) -o makePNG makePNG.o $(LIBPATH) $(LIBS)

install:
	install -m 0755 libKeyFrame.so /usr/lib/libKeyFrame.so
	install -m 0644 libKeyFrame.h /usr/include/libKeyFrame.h

run:
	LD_LIBRARY_PATH=. ./test

clean: 
	rm -rf *.o *.so *.png *.pgm test
