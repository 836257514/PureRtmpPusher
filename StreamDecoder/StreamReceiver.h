#pragma once
#include <iostream>
#include "StatusCode.h"
#include <string>
#include "VideoFrame.h"
#include <thread>
#include <windows.h>

extern "C"
{
	#include <libavformat\avformat.h>
	#include <libavutil\imgutils.h>
	#include <libswscale\swscale.h>
	#include <libavdevice\avdevice.h>

}

typedef void(__stdcall *FrameReceived)(VideoFrame& videoFrame);

class StreamReceiver
{
private:
	AVFormatContext* m_avFormatContext;
	AVCodecContext* m_avCodecContext;
	AVPixelFormat m_srcPixelFormat;
	char* m_streamUrl;
	int m_timeOut; //micro second
	AVPixelFormat m_targetPixelFormat;
	SwsContext* m_swsContext;
	int m_videoIndex;
	FrameReceived m_frameReceivedCB;
	bool m_stopReceive;
	std::thread m_receiveThread;
	void receive();
public:
	StreamReceiver(const char* streamUrl, int timeOut, InputPixelFormat& targetPixelFormat, FrameReceived frameReceived);
	~StreamReceiver();
	StatusCode init(bool enableHardwareDecode = false);
	void keep_receive();
};

