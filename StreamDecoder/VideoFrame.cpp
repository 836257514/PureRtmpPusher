#include "pch.h"
#include "VideoFrame.h"


VideoFrame::VideoFrame(AVFrame* frame, int index)
{
	m_width = frame->width;
	m_height = frame->height;
	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
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
	for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
	{
		m_data[i] = frame->data[i];
	}
}