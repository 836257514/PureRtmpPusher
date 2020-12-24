#pragma once
#include <libavdevice\avdevice.h>
#include <string>
#include <map>
using namespace std;

struct CameraData
{
    int width;
    int height;
    int videoStreamIndex;
    AVPixelFormat pixelFormat;
};

struct VideoConfig
{
    char* inputUrl;
    map<string, string> map;
};

struct AVFramePair
{
    AVFrame* srcAVFrame;
    //if the codec is qsv then this should be nv12 frame type, other it's yuvi420
    AVFrame* outputYuvFrame;
};

