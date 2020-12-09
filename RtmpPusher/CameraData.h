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
    const char* inputUrl;
    //video config map;
    map<string, string> map;
};

struct AVFramePair
{
    AVFrame* srcAVFrame;
    AVFrame* yuv420Frame;
};

