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
	AVPixelFormat m_targetPixelFormat;
	SwsContext* m_swsContext;
	int m_videoIndex;
public:
	StatusCode init(const char* streamUrl, int timeOut, AVPixelFormat& targetPixelFormat);
	void receive();
};

