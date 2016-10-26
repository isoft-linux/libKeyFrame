# use pkg-config for getting CFLAGS and LDLIBS
FFMPEG_LIBS=    libavdevice                        \
                libavformat                        \
                libavfilter                        \
                libavcodec                         \
                libswresample                      \
                libswscale                         \
                libavutil                          \

CC		= gcc
CFLAGS	+= -O2 -Wall -fPIC -std=gnu11
#CFLAGS  += -g -DDEBUG
CFLAGS	:= $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
CPATH	=
LIBPATH	=
LIBS	+= -lm
LIBS 	:= $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)

all: libKeyFrame

libKeyFrame:
	$(CC) -o libKeyFrame.o -c $(CFLAGS) $(CPATH) libKeyFrame.c
	$(CC) -o libKeyFrame.so libKeyFrame.o $(LIBPATH) $(LIBS) -shared

test:
	$(CC) -o test.o -c $(CFLAGS) $(CPATH) test.c
	$(CC) -o test test.o libKeyFrame.o $(LIBPATH) $(LIBS) -lKeyFrame

install:
	install -m 0755 libKeyFrame.so /usr/lib/libKeyFrame.so
	install -m 0644 libKeyFrame.h /usr/include/libKeyFrame.h

clean: 
	rm -rf *.o *.so test
