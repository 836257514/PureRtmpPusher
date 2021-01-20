#include "pch.h"
#include "StreamReceiver.h"
#include <windows.h>
#include "ImageFormatConverter.h"

using namespace std;

StreamReceiver::StreamReceiver(const char* streamUrl, int timeOut, InputPixelFormat& targetPixelFormat, FrameReceived frameReceived)
{
	int length = strlen(streamUrl) + 1;
	m_streamUrl = new char[length];
	strcpy_s(m_streamUrl, length, streamUrl);
	m_timeOut = timeOut;
	m_targetPixelFormat = ImageFormatConverter::convert_input_format(targetPixelFormat);
	m_frameReceivedCB = frameReceived;
}

StreamReceiver::~StreamReceiver()
{
	if (m_avFormatContext != nullptr)
	{
		avformat_free_context(m_avFormatContext);
		m_avFormatContext = nullptr;
	}

	if (m_avCodecContext != nullptr)
	{
		avcodec_free_context(&m_avCodecContext);
		m_avCodecContext = nullptr;
	}

	if (m_swsContext != nullptr)
	{
		sws_freeContext(m_swsContext);
		m_swsContext = nullptr;
	}

	delete[] m_streamUrl;
}

StatusCode StreamReceiver::init()
{
	avdevice_register_all();
	avformat_network_init();
	AVDictionary* pOptions = nullptr;
	const char* cTimeOut = std::to_string(m_timeOut).c_str();
	av_dict_set(&pOptions, "stimeout", cTimeOut, 0);
	int ret = avformat_open_input(&m_avFormatContext, m_streamUrl, NULL, &pOptions);
	if (ret != 0)
	{
		return StatusCode::OpenInputFail;
	}

	ret = avformat_find_stream_info(m_avFormatContext, NULL);
	if (ret < 0)
	{
		return StatusCode::FindStreamInfoFail;
	}

	AVCodecParameters* pAvCodecParameter = NULL;
	for (int i = 0; i < m_avFormatContext->nb_streams; ++i)
	{
		if (m_avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoIndex = i;
			pAvCodecParameter = m_avFormatContext->streams[i]->codecpar;
			break;
		}
	}
	if (pAvCodecParameter == NULL)
	{
		return StatusCode::CanNotFindVideoStream;
	}

	m_srcPixelFormat = ImageFormatConverter::convert_deprecated_format((AVPixelFormat)pAvCodecParameter->format);

	AVCodec* pCodec = avcodec_find_decoder(pAvCodecParameter->codec_id);
	if (pCodec == NULL)
	{
		return StatusCode::CanNotFindDecoder;
	}

	m_avCodecContext = avcodec_alloc_context3(pCodec);
	ret = avcodec_parameters_to_context(m_avCodecContext, pAvCodecParameter);
	if (ret < 0)
	{
		return StatusCode::CodecParameterToContextFail;
	}

	ret = avcodec_open2(m_avCodecContext, pCodec, NULL);
	if (ret < 0)
	{
		return StatusCode::DecoderToContextFail;
	}

	m_swsContext = sws_getContext(m_avCodecContext->width,
		m_avCodecContext->height,
		m_avCodecContext->pix_fmt,
		m_avCodecContext->width,
		m_avCodecContext->height,
		m_targetPixelFormat,
		SWS_BICUBIC,
		NULL,
		NULL,
		NULL);

	return StatusCode::Success;
}

void StreamReceiver::receive()
{
	AVFrame* pSrcFrame = av_frame_alloc();
	AVPacket* pAvPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
	AVFrame* pTargetFrame = NULL;
	VideoFrame pVideoFrame(m_avCodecContext->width, m_avCodecContext->height, m_targetPixelFormat, 0);
	if (m_srcPixelFormat != m_targetPixelFormat)
	{
		pTargetFrame = av_frame_alloc();
		pTargetFrame->format = m_targetPixelFormat;
		pTargetFrame->width = m_avCodecContext->width;
		pTargetFrame->height = m_avCodecContext->height;
		av_image_alloc(pTargetFrame->data, pTargetFrame->linesize, m_avCodecContext->width, m_avCodecContext->height, m_targetPixelFormat, 1);
		pVideoFrame.set_data(pTargetFrame->data);
	}

	while (true)
	{
		int ret = av_read_frame(m_avFormatContext, pAvPacket);
		if (ret == 0)
		{
			//only care about video frame.
			if (pAvPacket->stream_index == m_videoIndex)
			{
				int ret = avcodec_send_packet(m_avCodecContext, pAvPacket);
				av_packet_unref(pAvPacket);
				if (ret != 0)
				{
					break;
				}

				ret = avcodec_receive_frame(m_avCodecContext, pSrcFrame);
				if (ret == -11)
				{
					continue;
				}

				if (ret != 0)
				{
					break;
				}

				//if use target frame then the data address is same.
				if (m_srcPixelFormat != m_targetPixelFormat)
				{
					sws_scale(m_swsContext, (const uint8_t* const*)pSrcFrame->data, pSrcFrame->linesize, 0, m_avCodecContext->height, pTargetFrame->data, pTargetFrame->linesize);
				}
				else
				{
					//if use src frame then the data address is different.
					pVideoFrame.set_data(pSrcFrame->data);
				}
				
				m_frameReceivedCB(pVideoFrame);
				pVideoFrame.m_index++;
			}
		}
		Sleep(2);
	}

	av_packet_unref(pAvPacket);
	av_frame_unref(pSrcFrame);
	if (pTargetFrame != NULL)
	{
		av_frame_unref(pTargetFrame);
	}
}