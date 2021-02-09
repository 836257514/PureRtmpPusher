#pragma once
#include "CameraData.h"
#include <functional>
#include "PushConfigCommon.h"
#include "Logger.h"
#include "ImgUtility.h"
#define  LIVESTREAM_BITRATE_720P 800000
extern "C"
{
#include <libavformat\avformat.h>
#include <libavutil\opt.h>
}

typedef std::function<void(AVPacket * avPacket)> H264EncodedCallBack;

class Encoder
{
public:
	Encoder(PushConfig& pushConfig, H264EncodedCallBack callBack, bool enableHardwareEncoder = false);
	~Encoder();
	void encode_frame(AVFrame* frame);
	AVPixelFormat get_input_image_format();
private:
	AVPacket* m_avPacket;
	H264EncodedCallBack m_h264CallBack;
	AVCodecContext* m_avCodecContext;
	static int get_bitrate(int width, int height, int frameRate);
	AVCodecContext* get_hardware_codec(int width, int height, int frameRate);
	AVCodecContext* get_software_codec(int width, int height, int frameRate);
};

