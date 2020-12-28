#include "pch.h"
#include "StreamReceiver.h"

StatusCode StreamReceiver::init(const char* streamUrl, int timeOut, AVPixelFormat& targetPixelFormat)
{
    m_targetPixelFormat = targetPixelFormat;
    avformat_network_init();
    AVDictionary* pOptions;
    const char* cTimeOut = std::to_string(timeOut).c_str();
    av_dict_set(&pOptions, "stimeout", cTimeOut, 0);
    int ret = avformat_open_input(&m_avFormatContext, streamUrl, NULL, &pOptions);
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
    AVFrame* srcFrame = av_frame_alloc();
    AVFrame* targetFrame = av_frame_alloc();
    av_image_alloc(targetFrame->data, targetFrame->linesize, m_avCodecContext->width, m_avCodecContext->height, m_targetPixelFormat, 1);
    AVPacket* pAvPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
    while (av_read_frame(m_avFormatContext, pAvPacket) == 0)
    {
        //only care about video frame.
        if (pAvPacket->stream_index == m_videoIndex)
        {
            int ret = avcodec_send_packet(m_avCodecContext, pAvPacket);
            if (ret != 0)
            {
                break;
            }

            ret = avcodec_receive_frame(m_avCodecContext, srcFrame);
            if (ret != 0)
            {
                break;
            }

            sws_scale(m_swsContext, (const uint8_t* const*)srcFrame->data, srcFrame->linesize, 0, m_avCodecContext->height, targetFrame->data, targetFrame->linesize);
        }
        av_packet_unref(pAvPacket);
    }

    av_frame_unref(srcFrame);
    av_frame_unref(targetFrame);
}