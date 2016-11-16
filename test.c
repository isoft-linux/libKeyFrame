#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libKeyFrame.h"

int main(int argc, char *argv[]) 
{
    /* 
     * ffmpeg v2.8 libavformat's corrupted double-linked list issue

#0  __GI_raise (sig=sig@entry=6) at ../sysdeps/unix/sysv/linux/raise.c:58
#1  0x00007ffff53f923a in __GI_abort () at abort.c:89
#2  0x00007ffff5439b80 in __libc_message (do_abort=do_abort@entry=2, 
    fmt=fmt@entry=0x7ffff554a168 "*** Error in `%s': %s: 0x%s ***\n")
    at ../sysdeps/posix/libc_fatal.c:175
#3  0x00007ffff5443564 in malloc_printerr (ar_ptr=0x7ffff577ab20 <main_arena>, ptr=<optimized out>, 
    str=0x7ffff554782a "corrupted double-linked list", action=3) at malloc.c:5046
#4  _int_free (av=0x7ffff577ab20 <main_arena>, p=0x62bb10, have_lock=0) at malloc.c:4049
#5  0x00007ffff544648c in __GI___libc_free (mem=<optimized out>) at malloc.c:2982
#6  0x00007ffff771f33f in ff_free_stream () from /lib/libavformat.so.56
#7  0x00007ffff771f487 in avformat_free_context () from /lib/libavformat.so.56
#8  0x00007ffff772000d in avformat_open_input () from /lib/libavformat.so.56
#9  0x00007ffff7bd65ff in findKeyFrame (
    src_file=src_file@entry=0x400758 "/data/download/20001134720_7618331.mp4", 
    out_dir=out_dir@entry=0x400734 "/tmp", img_width=img_width@entry=0, img_height=img_height@entry=0)
    at libKeyFrame.c:346
#10 0x000000000040059a in main (argc=<optimized out>, argv=<optimized out>) at test.c:10

     * *BUT* ffmpeg v3.2 is OK!
     */
    findKeyFrame("/data/download/isoft.avi", "/tmp", 0, 0);
    findKeyFrame("/data/download/20001134720_7618331.mp4", "/tmp", 0, 0);
    return 0;
}
