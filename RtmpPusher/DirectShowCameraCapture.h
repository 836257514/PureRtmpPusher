#pragma once
#include <iostream>
#include <dshow.h>
#include <vector>
#include <map>
#include "Logger.h"
#include "DeviceInfo.h"
#include "qedit.h"
#include "SampleGrabberCallback.h"
#include "PushConfigCommon.h"
#include "ImageCapturer.h"

using namespace std;

class DirectShowCameraCapture
{
private:
    IEnumMoniker* m_pEnumMoniker;
    IMediaControl* m_pMediaControl;
    SampleGrabberCallback m_sampleGrabberCallBack;
    IBaseFilter* m_pFrameGrabberFilter;
    IGraphBuilder* m_pGraph; // graph manager
    ICaptureGraphBuilder2* m_pCaptureGraphBuilder2;
    GUID m_outputFormat;
public:
    DirectShowCameraCapture(GUID outputFormat);
    ~DirectShowCameraCapture();
public:
    HRESULT get_camera_list(map<int, DeviceInfo>&);
    HRESULT capture(DeviceInfo&);
    void set_encode_callBack(ImageCallBack& imageCaptureCB, CaptureData& cameraData);
};

