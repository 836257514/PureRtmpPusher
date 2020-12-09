#include "ImageCapturer.h"
#include <thread>
#include <iostream>

ImageCapturer::ImageCapturer(ImageCallBack imageCaptureCB)
{
	m_imageCapturedCB = imageCaptureCB;
}

int ImageCapturer::io_interrupt_callBack(void* objectPointer)
{
	return 0;
}

bool ImageCapturer::open_camera(VideoConfig& videoConfig)
{
	m_cameraData.videoStreamIndex = -1;
	avdevice_register_all();
	m_avFormatContext = avformat_alloc_context();
	m_avFormatContext->interrupt_callback.callback = io_interrupt_callBack;
	AVInputFormat* ifmt = av_find_input_format("dshow");
	AVDictionary* format_opts = nullptr;
	map<string, string>::iterator iterator;
	for (iterator = videoConfig.map.begin(); iterator != videoConfig.map.end(); ++iterator)
	{
		av_dict_set(&format_opts, iterator->first.c_str(), iterator->second.c_str(), 0);
	}
	av_dict_set_int(&format_opts, "rtbufsize", 3041280 * 10, 0);

	int ret = avformat_open_input(&m_avFormatContext, videoConfig.inputUrl, ifmt, &format_opts);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Input file open input failed\n");
		return  false;
	}
	ret = avformat_find_stream_info(m_avFormatContext, nullptr);
	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Find input file stream inform failed\n");
		return  false;
	}

	for (int i = 0; i < m_avFormatContext->nb_streams; i++)
	{
		if (m_avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			if (init_camera_data(i))
			{
				return true;
			}
		}
	}
	
	return false;
}

bool ImageCapturer::init_camera_data(int streamIndex)
{
	AVCodecParameters* codecParameters = m_avFormatContext->streams[streamIndex]->codecpar;
	//摄像头应该是MJpeg流/或者YUV裸流
	AVCodec* decoder = avcodec_find_decoder(codecParameters->codec_id);
	if (decoder == nullptr)
	{
		av_log(NULL, AV_LOG_ERROR, "Couldn't find a video decoder.（没有找到解码器）\n");
		return false;
	}

	m_codecContext = avcodec_alloc_context3(decoder);
	if (avcodec_open2(m_codecContext, decoder, NULL) < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "Could not open codec.（无法打开解码器）\n");
		return false;
	}

	m_cameraData.videoStreamIndex = streamIndex;
	m_cameraData.height = codecParameters->height;
	m_cameraData.width = codecParameters->width;
	m_cameraData.pixelFormat = ImgUtility::convert_deprecated_format((AVPixelFormat)codecParameters->format);
	return true;
}

bool ImageCapturer::malloc_frame(AVFramePair& pair)
{
	pair.srcAVFrame = av_frame_alloc();
	if (!pair.srcAVFrame)
	{
		return false;
	}
	pair.yuv420Frame = av_frame_alloc();
	if (!pair.yuv420Frame)
	{
		return false;
	}

	pair.yuv420Frame->width = m_cameraData.width;
	pair.yuv420Frame->height = m_cameraData.height;
	pair.yuv420Frame->linesize[0] = m_cameraData.width * m_cameraData.height * 1.5;
	pair.yuv420Frame->format = AV_PIX_FMT_YUV420P;
	av_image_alloc(pair.yuv420Frame->data, pair.yuv420Frame->linesize, pair.yuv420Frame->width, pair.yuv420Frame->height, AV_PIX_FMT_YUV420P, 1);
	m_swsContext = sws_getContext(m_cameraData.width, m_cameraData.height, m_cameraData.pixelFormat, m_cameraData.width, m_cameraData.height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr, nullptr, nullptr);
	return true;
}

void ImageCapturer::read()
{
	AVFramePair pair;
	if (!malloc_frame(pair))
	{
		return;
	}

	pair.yuv420Frame->pts = 0;
	AVPacket packet;
	__try
	{
		while (true)
		{
			//read next one into packet.
			int ret = av_read_frame(m_avFormatContext, &packet);
			if (ret < 0 || packet.stream_index != m_cameraData.videoStreamIndex)
			{
				__leave;
			}

			//send into decode queue.
			ret = avcodec_send_packet(m_codecContext, &packet);
			if (ret < 0)
			{
				__leave;
			}

			//get decoded frame.
			ret = avcodec_receive_frame(m_codecContext, pair.srcAVFrame);
			if (ret < 0)
			{
				__leave;
			}

			ret = sws_scale(m_swsContext, pair.srcAVFrame->data, pair.srcAVFrame->linesize, 0, pair.srcAVFrame->height,
				pair.yuv420Frame->data, pair.yuv420Frame->linesize);
			pair.yuv420Frame->pts++;
			m_imageCapturedCB(pair.yuv420Frame);
		}
	}
	__finally
	{
		av_frame_unref(pair.srcAVFrame);
		av_frame_unref(pair.yuv420Frame);
		av_packet_unref(&packet);
	}
}

ImageCapturer::~ImageCapturer()
{
	if (m_avFormatContext != nullptr)
	{
		avformat_free_context(m_avFormatContext);
		m_avFormatContext = nullptr;
	}

	if (m_swsContext != nullptr)
	{
		sws_freeContext(m_swsContext);
		m_swsContext = nullptr;
	}
}