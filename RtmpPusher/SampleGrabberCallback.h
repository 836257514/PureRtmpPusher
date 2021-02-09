#ifndef SAMPLEGRABBERCALLBACK_H
#define SAMPLEGRABBERCALLBACK_H
#include <dshow.h>
#include "qedit.h"
#include <iostream>
#include <string>
#include <mutex>
#include "CameraData.h"
#include "Logger.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/imgutils.h>
}

using namespace std;

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
    SampleGrabberCallback() : m_swsContext(nullptr) {}
public:
    HRESULT SampleCB(double SampleTime, IMediaSample* pSample);
    HRESULT BufferCB(double SampleTime, unsigned char* pBuffer, long BufferLen);
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) { return 1; }
    ULONG STDMETHODCALLTYPE AddRef(void) { return 1; }
    ULONG STDMETHODCALLTYPE Release(void) { return 1; }
    void SetCallback(ImageCallBack& imageCaptureCB, CaptureData& captureData);
};

#endif // SAMPLEGRABBERCALLBACK_H