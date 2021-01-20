#include "pch.h"
#include "VideoFrame.h"


VideoFrame::VideoFrame(int width, int height, AVPixelFormat format, int index)
{
	m_width = width;
	m_height = height;
	switch (format)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_NV12:
		m_length = m_width * m_height * 1.5;
		break;
	case AV_PIX_FMT_BGR24:
		m_length = m_width * m_height * 3;
		break;
	case AV_PIX_FMT_BGRA:
		m_length = m_width * m_height * 4;
		break;
	}
	m_index = index;
}

void VideoFrame::set_data(uint8_t* data[AV_NUM_DATA_POINTERS])
{
	for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
	{
		if (m_data[i] != data[i])
		{
			m_data[i] = data[i];
		}
	}
}