#include "Encoder.h"

Encoder::Encoder(PushConfig& config, H264EncodedCallBack callBack, bool enableHardwareEncoder)
{
	m_avPacket = ImgUtility::create_packet();
	if (m_avPacket == NULL)
	{
		Logger::write("create packet fail when encode");
	}
	m_h264CallBack = callBack;
	if (enableHardwareEncoder)
	{
		m_avCodecContext = get_hardware_codec(config.width, config.height, config.frameRate);
		if (m_avCodecContext == NULL)
		{
			throw std::exception("Cann't find qsv encoder");
		}
	}
	else
	{
		m_avCodecContext = get_software_codec(config.width, config.height, config.frameRate);
	}
}

Encoder::~Encoder()
{
	if (m_avCodecContext != nullptr)
	{
		avcodec_free_context(&m_avCodecContext);
		m_avCodecContext = nullptr;
	}
}

AVPixelFormat Encoder::get_input_image_format()
{
	return m_avCodecContext->pix_fmt;
}

void Encoder::encode_frame(AVFrame* frame)
{
	// send frame to encode queue;
	if (avcodec_send_frame(m_avCodecContext, frame) == 0)
	{
		//receive packet from encode queue.
		int ret = avcodec_receive_packet(m_avCodecContext, m_avPacket);
		if (ret == 0)
		{
			m_h264CallBack(m_avPacket);
			av_packet_unref(m_avPacket);
		}
	}
}

AVCodecContext* Encoder::get_hardware_codec(int width, int height, int frameRate)
{
	avdevice_register_all();
	AVCodec* h264Codec = avcodec_find_encoder_by_name("h264_qsv");
	if (h264Codec == nullptr)
	{
		return nullptr;
	}
	AVCodecContext* codecContext = avcodec_alloc_context3(h264Codec);
	if (codecContext == nullptr)
	{
		return nullptr;
	}

	//codecContext->codec_id = h264Codec->id;
	codecContext->width = width; //Width
	codecContext->height = height; //Height
	codecContext->time_base = { 1, frameRate };  //frames per second
	codecContext->pix_fmt = *(h264Codec->pix_fmts); //nv12
	//av_opt_set(codecContext->priv_data, "profile", "main", 0);
	codecContext->bit_rate = get_bitrate(width, height, frameRate);// put sample parameters   
	codecContext->gop_size = 12; //   
	codecContext->max_b_frames = 1; //B frames

	//av_dict_set(&options, "profile", "baseline", 0);

	if (avcodec_open2(codecContext, h264Codec, nullptr) < 0)
	{
		avcodec_free_context(&codecContext);
		return nullptr;
	}
	return codecContext;
}

AVCodecContext* Encoder::get_software_codec(int width, int height, int frameRate)
{
	AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (codec == nullptr)
	{
		return nullptr;
	}
	AVCodecContext* codecContext = avcodec_alloc_context3(codec);
	if (codecContext == nullptr)
	{
		return nullptr;
	}

	codecContext->width = width; //Width
	codecContext->height = height; //Height
	codecContext->time_base = { 1, frameRate };  //frames per second
	codecContext->thread_count = 2; //Thread used.
	codecContext->pix_fmt = *codec->pix_fmts;//AV_PIX_FMT_YUV420P; //YUV420 for H264
	codecContext->bit_rate = get_bitrate(width, height, frameRate);// put sample parameters   
	codecContext->gop_size = frameRate * 2; // emit one intra frame every ten frames   
	codecContext->max_b_frames = 1; //B frames
	//Set parameters
	AVDictionary* options = nullptr;
	//基本画质。支持I/P 帧，只支持无交错（Progressive）和CAVLC
	av_dict_set(&options, "profile", "baseline", 0);
	//低质量，快编码速度
	av_dict_set(&options, "preset", "fast", 0);

	if (avcodec_open2(codecContext, codec, &options) < 0)
	{
		av_dict_free(&options);
		avcodec_free_context(&codecContext);
		return nullptr;
	}
	return codecContext;
}

int Encoder::get_bitrate(int width, int height, int frameRate)
{
	if (width == 0 || height == 0 || frameRate == 0)
	{
		return  LIVESTREAM_BITRATE_720P;
	}

	int bitrate = LIVESTREAM_BITRATE_720P * width / 1280;
	bitrate = bitrate * height / 720;
	if (frameRate >= 15)
	{
		bitrate = bitrate * frameRate / 20;
	}
	else if (frameRate < 15)
	{
		bitrate = bitrate * 15 / 20;
	}

	return bitrate;
}