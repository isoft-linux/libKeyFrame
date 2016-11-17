#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libKeyFrame.h"

int main(int argc, char *argv[]) 
{
    findKeyFrame("/data/download/isoft.avi", "/tmp", 0, 0);
    findKeyFrame("/data/download/20001134720_7618331.mp4", "/tmp", 0, 0);
    findKeyFrame("/data/download/vmwview4.mov", "/tmp", 0, 0);
    findKeyFrame("/data/download/PEX2010_intro.mpg", "/tmp", 320, 0);
    findKeyFrame("/data/download/VMW_view4.f4v", "/tmp", 0, 0);
    findKeyFrame("/data/download/shahua.wmv", "/tmp", 0, 0);
    findKeyFrame("/data/download/sushi.m4v", "/tmp", 0, 0);
    findKeyFrame("/data/download/test.rmvb", "/tmp", 320, 0);
    return 0;
}
