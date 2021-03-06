#pragma once
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/imgutils.h"
}

#include "Encoder.h"
#include <functional>
#include <string>
#include "ImgUtility.h"
#include "Logger.h"
#include <map>
#include "SampleGrabberCallback.h"
using namespace std;

struct VideoConfig
{
	char* inputUrl;
	map<string, string> map;
};

struct CameraData
{
	int width;
	int height;
	int videoStreamIndex;
	AVPixelFormat pixelFormat;
};

class ImageCapturer
{
private:
	ImageCallBack m_imageCapturedCB;
	CameraData m_cameraData;
	AVFormatContext* m_avFormatContext;
	AVCodecContext* m_codecContext;
	SwsContext* m_swsContext;
	AVPixelFormat m_avPixelOutputFormat;
	bool init_camera_data(int streamIndex);
	bool malloc_frame(AVFramePair& pair);
	static int m_timeout;
	static int io_interrupt_callBack(void* objectPointer);
public:
	ImageCapturer(ImageCallBack imageCaptureCB, AVPixelFormat imageOutputFormat);
	~ImageCapturer();
	bool open_camera(VideoConfig& videoConfig);
	void read();
};