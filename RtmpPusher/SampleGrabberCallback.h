#ifndef SAMPLEGRABBERCALLBACK_H
#define SAMPLEGRABBERCALLBACK_H
#include <dshow.h>
#include "qedit.h"
#include <iostream>
#include <string>
#include <mutex>
#include <functional>
#include "Logger.h"
#include "PushConfigCommon.h"
#include "DeviceInfo.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
}

using namespace std;
typedef std::function<void(AVFrame* frame)> ImageCallBack;

struct AVFramePair
{
    AVFrame* srcAVFrame;
    //if the codec is qsv then this should be nv12 frame type, other it's yuvi420
    AVFrame* outputYuvFrame;
};

struct CaptureInfo
{
    int width;
    int height;
    CaptureFormat inputFormat;
    AVPixelFormat outputFormat;
    CaptureInfo(int w, int h, CaptureFormat iinputFormat, AVPixelFormat oOutputFormat)
        : width(w), height(h), inputFormat(iinputFormat), outputFormat(oOutputFormat) {}
};

enum class GrabberCallBackCategory
{
    SampleCB,
    BufferCB
};

class SampleGrabberCallback : public ISampleGrabberCB
{
private:
    ImageCallBack m_imageCallback;
    SwsContext* m_swsContext;
    AVFramePair m_avFramePair;
public:
    SampleGrabberCallback();
    ~SampleGrabberCallback();
public:
    HRESULT SampleCB(double SampleTime, IMediaSample* pSample);
    HRESULT BufferCB(double SampleTime, unsigned char* pBuffer, long BufferLen);
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) { return 1; }
    ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
    ULONG STDMETHODCALLTYPE Release(void) { return 1; }
    void SetCallback(ImageCallBack& imageCaptureCB, CaptureInfo& captureData);
};

#endif // SAMPLEGRABBERCALLBACK_H