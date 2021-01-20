#pragma once
#include <vector>

extern "C"
{
#include <libavutil\frame.h>
}



class VideoFrame
{
public:
	int m_width;
	int m_height;
	int m_length;
	int m_index;
	uint8_t* m_data[AV_NUM_DATA_POINTERS];
	VideoFrame(int width, int height, AVPixelFormat format, int index);
	void set_data(uint8_t* m_data[AV_NUM_DATA_POINTERS]);
};

