#pragma once
#include <iostream>
#include "StatusCode.h"
#include <string>
extern "C"
{
	#include <libavformat\avformat.h>
	#include <libavutil\imgutils.h>
	#include <libswscale\swscale.h>
}


class StreamReceiver
{
private:
	AVFormatContext* m_avFormatContext;
	AVCodecContext* m_avCodecContext;
	const char* m_streamUrl;
	int m_timeOut;
	AVPixelFormat m_targetPixelFormat;
	SwsContext* m_swsContext;
	int m_videoIndex;
public:
	StreamReceiver(const char* streamUrl, int timeOut, ConvertPixelFormat& targetPixelFormat);
	~StreamReceiver();
	StatusCode init();
	void receive();
};

