#include <stdio.h>

#include "libKeyFrame.h"

int main(int argc, char *argv[]) 
{
    findKeyFrame(argv[1] ? argv[1] : "isoft.avi", "/tmp");
    return 0;
}
