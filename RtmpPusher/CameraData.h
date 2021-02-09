#pragma once
extern "C"
{
#include <libavdevice\avdevice.h>
}

#include <string>
#include <functional>
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

struct CaptureData
{
    int width;
    int height;
    AVPixelFormat outputFormat;

    CaptureData(int w, int h, AVPixelFormat format) : width(w), height(h), outputFormat(format) {}
};

//just shorthand to avoid long typing
typedef std::function<void(AVFrame * frame)> ImageCallBack;

