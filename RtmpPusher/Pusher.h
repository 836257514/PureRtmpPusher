#pragma once
#include <string>
#include "PushConfigCommon.h"
extern "C"
{
#include "libavcodec\codec_id.h"
#include "libavformat\avformat.h"
#include "libavutil/time.h"
}

using namespace std;

class Pusher
{
private:
	AVFormatContext* m_formatContext;
	AVCodecContext* m_codecContext;
	int64_t m_startTime;
public:
	~Pusher();
	void set_config(PushConfig pushConfig);
	void push(AVPacket* avPacket);
};

