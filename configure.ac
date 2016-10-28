AC_INIT([libKeyFrame], [1.0], [xiang.zhai@i-soft.com.cn])
AM_INIT_AUTOMAKE([subdir-objects -Wall foreign])
AC_PROG_CC
AM_PROG_AR
AM_PROG_LIBTOOL
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])
LT_INIT

FFMPEG_REQUIRES="libavdevice >= 56.4.100 libavformat >= 56.40.101 libavfilter >= 5.40.101 libavcodec >= 56.60.100 libswresample >= 1.2.101 libswscale >= 3.1.101 libavutil >= 54.31.100"
PKG_CHECK_MODULES(FFMPEG, $FFMPEG_REQUIRES)
AC_SUBST(FFMPEG_CFLAGS)
AC_SUBST(FFMPEG_LIBS)

LIBKEYFRAME_REQUIRES=$FFMPEG_REQUIRES
AC_SUBST(LIBKEYFRAME_REQUIRES)

AC_ARG_ENABLE(debug,
              [AS_HELP_STRING([--enable-debug=@<:@no/yes@:>@],
                              [turn on debugging [default=no]])],,
              [enable_debug=no])

if test "x$enable_debug" = "xyes"; then
  test "$cflags_set" = set || CFLAGS="$CFLAGS -g -DDEBUG"
fi

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 libKeyFrame.pc
])
AC_OUTPUT
