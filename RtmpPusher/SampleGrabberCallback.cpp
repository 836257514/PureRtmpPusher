#include "SampleGrabberCallback.h"


SampleGrabberCallback::SampleGrabberCallback() : m_swsContext(nullptr) 
{
}

SampleGrabberCallback::~SampleGrabberCallback()
{
	if (m_swsContext)
	{
		sws_freeContext(m_swsContext);
		m_swsContext = nullptr;
	}
}

HRESULT SampleGrabberCallback::SampleCB(double sampleTime, IMediaSample* pSample) {
    return 1;
}

HRESULT SampleGrabberCallback::BufferCB(double sampleTime, unsigned char* pBuffer, long bufferLength) {
    if (pBuffer) {

		//Flip
		int stride = m_avFramePair.srcAVFrame->linesize[0];
		pBuffer += stride * (m_avFramePair.srcAVFrame->height - 1);
		for (int i = 0; i < m_avFramePair.srcAVFrame->height; ++i)
		{
			memcpy_s(m_avFramePair.srcAVFrame->data[0] + i * stride, bufferLength, pBuffer, stride);
			pBuffer -= stride;
		}
		
		int ret = sws_scale(m_swsContext, m_avFramePair.srcAVFrame->data, m_avFramePair.srcAVFrame->linesize, 0, m_avFramePair.srcAVFrame->height,
			m_avFramePair.outputYuvFrame->data, m_avFramePair.outputYuvFrame->linesize);
		if (ret <= 0)
		{
			Logger::write("sws image failed", ret);
			return E_FAIL;
		}

		m_avFramePair.outputYuvFrame->pts++;
		m_imageCallback(m_avFramePair.outputYuvFrame);
    }

    return 1;
}

void SampleGrabberCallback::SetCallback(ImageCallBack& imageCaptureCB, CaptureInfo& captureData)
{
	m_imageCallback = imageCaptureCB;
	m_avFramePair.srcAVFrame = av_frame_alloc();
	if (!m_avFramePair.srcAVFrame)
	{
		return;
	}

	m_avFramePair.srcAVFrame->width = captureData.width;
	m_avFramePair.srcAVFrame->height = captureData.height;
	AVPixelFormat inputFormat = AV_PIX_FMT_BGR24;
	if (captureData.inputFormat == CaptureFormat::MJpeg)
	{
		inputFormat = AV_PIX_FMT_BGR24;
		m_avFramePair.srcAVFrame->linesize[0] = captureData.width * 3;
	}

	if (captureData.inputFormat == CaptureFormat::YUY2)
	{
		inputFormat = AV_PIX_FMT_YUYV422;
		m_avFramePair.srcAVFrame->linesize[0] = captureData.width * 2;
	}
	
	m_avFramePair.srcAVFrame->format = inputFormat;
	av_image_alloc(m_avFramePair.srcAVFrame->data, m_avFramePair.srcAVFrame->linesize, m_avFramePair.srcAVFrame->width, m_avFramePair.srcAVFrame->height, AV_PIX_FMT_RGB24, 1);

	m_avFramePair.outputYuvFrame = av_frame_alloc();
	if (!m_avFramePair.outputYuvFrame)
	{
		return;
	}

	m_avFramePair.outputYuvFrame->width = captureData.width;
	m_avFramePair.outputYuvFrame->height = captureData.height;
	if (captureData.outputFormat == AV_PIX_FMT_YUV420P)
	{
		m_avFramePair.outputYuvFrame->linesize[0] = captureData.width;
		m_avFramePair.outputYuvFrame->linesize[1] = captureData.width / 2;
		m_avFramePair.outputYuvFrame->linesize[2] = captureData.width / 2;
	}

	if (captureData.outputFormat == AV_PIX_FMT_NV12)
	{
		m_avFramePair.outputYuvFrame->linesize[0] = captureData.width;
		m_avFramePair.outputYuvFrame->linesize[1] = captureData.width;
	}

	if (captureData.outputFormat == AV_PIX_FMT_BGR24 || captureData.outputFormat == AV_PIX_FMT_RGB24)
	{
		m_avFramePair.outputYuvFrame->linesize[0] = captureData.width * 3; 
	}

	if (captureData.outputFormat == AV_PIX_FMT_BGRA || captureData.outputFormat == AV_PIX_FMT_RGBA)
	{
		m_avFramePair.outputYuvFrame->linesize[0] = captureData.width * 4;
	}
	
	m_avFramePair.outputYuvFrame->format = captureData.outputFormat;
	m_avFramePair.outputYuvFrame->pts = 0;
	av_image_alloc(m_avFramePair.outputYuvFrame->data, m_avFramePair.outputYuvFrame->linesize, m_avFramePair.outputYuvFrame->width, m_avFramePair.outputYuvFrame->height, captureData.outputFormat, 1);
	m_swsContext = sws_getContext(captureData.width, captureData.height, inputFormat, captureData.width, captureData.height, captureData.outputFormat, SWS_BILINEAR, nullptr, nullptr, nullptr);
}