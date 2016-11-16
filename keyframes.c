#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libKeyFrame.h"

int main(int argc, char *argv[]) 
{
    char *src = NULL;
    char *out = NULL;
    int width, height;

    if ((src = argv[1]) == NULL || (out = argv[2]) == NULL) {
        printf("Usage: keyframes /src/isoft.avi /out 320(Optional) 240(Optional)\n");
        return -1;
    }

    if (access(out, W_OK) == -1)
        out = "/tmp";

    width = argv[3] ? atoi(argv[3]) : 0;
    height = argv[4] ? atoi(argv[4]) : 0;
    return findKeyFrame(src, out, width, height);
}
