#pragma once
#include "ImageCapturer.h"
#include <functional>
#define  LIVESTREAM_BITRATE_720P 800000
extern "C"
{
#include "libavformat\avformat.h"
#include "libavutil\opt.h"
}

typedef std::function<void(AVPacket * avPacket)> H264EncodedCallBack;

class Encoder
{
public:
	Encoder(int width, int height, int frameRate, H264EncodedCallBack callBack);
	~Encoder();
	void encode_frame(AVFrame* frame);
private:
	H264EncodedCallBack m_h264CallBack;
	AVCodecContext* m_avCodecContext;
	static int get_bitrate(int width, int height, int frameRate);
	AVCodecContext* get_qsv_context(int width, int height, int frameRate);
	AVCodecContext* get_h264_context(int width, int height, int frameRate);
};

