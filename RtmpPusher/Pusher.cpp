#include "Pusher.h"
#include "ImgUtility.h"

Pusher::~Pusher()
{
	if (m_codecContext != nullptr)
	{
		avcodec_free_context(&m_codecContext);
		m_codecContext = nullptr;
	}

	if (m_formatContext != nullptr)
	{
		avformat_free_context(m_formatContext);
		m_formatContext = nullptr;
	}
}

void Pusher::set_config(PushConfig pushConfig)
{
	int ret = avformat_alloc_output_context2(&m_formatContext, nullptr, "flv", pushConfig.url.c_str()); //RTMP
	if (ret < 0) {
		return;
	}
	m_formatContext->oformat->video_codec = AV_CODEC_ID_H264;
	m_formatContext->oformat->audio_codec = AV_CODEC_ID_NONE;

	//Try find codec
	AVOutputFormat* oformat = m_formatContext->oformat;
	AVCodec* codec = avcodec_find_encoder(oformat->video_codec);
	if (codec == nullptr) {
		return;
	}

	AVStream* outStream = avformat_new_stream(m_formatContext, codec);
	if (outStream == nullptr) {
		return;
	}

	m_codecContext = avcodec_alloc_context3(codec);
	if (m_codecContext == nullptr) {
		return;
	}

	m_codecContext->codec_id = oformat->video_codec;
	m_codecContext->width = pushConfig.width;
	m_codecContext->height = pushConfig.height;
	m_codecContext->time_base = { 1, pushConfig.frameRate };
	m_codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

	if (oformat->flags & AVFMT_GLOBALHEADER)
		m_codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

	//Open codec
	if (avcodec_open2(m_codecContext, codec, nullptr) < 0) {
		return;
	}

	//Copy the settings of AVCodecContext
	ret = avcodec_parameters_from_context(outStream->codecpar, m_codecContext);
	if (ret < 0) {
		return;
	}
	outStream->codecpar->codec_tag = 0;

	//Open output URL
	if (!(oformat->flags & AVFMT_NOFILE))
	{
		ret = avio_open(&m_formatContext->pb, pushConfig.url.c_str(), AVIO_FLAG_WRITE);
		if (ret < 0) {
			return;
		}
	}

	//Write file header
	ret = avformat_write_header(m_formatContext, nullptr);
	if (ret < 0) {
		return;
	}

	m_startTime = av_gettime();
}

void Pusher::push(AVPacket* avPacket)
{
	////Delay if too fast, too fast may cause the server crash.
	AVRational time_base = m_codecContext->time_base;
	AVRational time_base_q = { 1, AV_TIME_BASE };
	int64_t pts_time = av_rescale_q(avPacket->dts, time_base, time_base_q);
	int64_t now_time = av_gettime() - m_startTime;
	//if (pts_time > now_time)
	//{
	//	av_usleep(static_cast<unsigned int>(pts_time - now_time));
	//}

	//Re calc the pts dts and duration.
	AVStream* outStream = m_formatContext->streams[0];
	avPacket->pts = av_rescale_q_rnd(avPacket->pts, m_codecContext->time_base, outStream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	avPacket->dts = av_rescale_q_rnd(avPacket->dts, m_codecContext->time_base, outStream->time_base, static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
	avPacket->duration = av_rescale_q(avPacket->duration, m_codecContext->time_base, outStream->time_base);

	AVPacket* clonedAvPacket = ImgUtility::create_packet();
	av_init_packet(clonedAvPacket);
	clonedAvPacket->data = nullptr;
	clonedAvPacket->size = 0;
	av_packet_copy_props(clonedAvPacket, avPacket);
	av_packet_ref(clonedAvPacket, avPacket);
	//Write the frame, notice that here must use av_interleaved_write_frame, it will automatic control the interval.
	//Notice2, after call av_interleaved_write_frame, the packet's data will be released.
	//int tickcount = GetTickCount();
	int ret = av_write_frame(m_formatContext, clonedAvPacket);
	av_packet_unref(clonedAvPacket);
}

