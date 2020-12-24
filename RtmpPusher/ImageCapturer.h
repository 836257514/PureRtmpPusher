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
#include "CameraData.h"
#include "ImgUtility.h"
using namespace std;

//just shorthand to avoid long typing
typedef std::function<void(AVFrame * frame)> ImageCallBack;

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
	static int io_interrupt_callBack(void* objectPointer);
public:
	ImageCapturer(ImageCallBack imageCaptureCB, AVPixelFormat imageOutputFormat);
	~ImageCapturer();
	bool open_camera(VideoConfig& videoConfig);
	void read();
};