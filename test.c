#include <stdio.h>
#include <unistd.h>

#include "libKeyFrame.h"

int main(int argc, char *argv[]) 
{
    char *src = argv[1] ? argv[1] : "isoft.avi";
    char *dir = argv[2] ? argv[2] : "/tmp";
    if (access(dir, W_OK) == -1)
        dir = "/tmp";
    printf("Seek %s key fram and save png to %s\n", src, dir);
    printf("%d\n", findKeyFrame(src, dir));
    return 0;
}
