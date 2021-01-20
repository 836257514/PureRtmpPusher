#include "pch.h"
#include "ImageFormatConverter.h"

AVPixelFormat ImageFormatConverter::convert_deprecated_format(AVPixelFormat format)
{
	switch (format)
	{
	case AV_PIX_FMT_YUVJ420P:
		return AV_PIX_FMT_YUV420P;
	case AV_PIX_FMT_YUVJ422P:
		return AV_PIX_FMT_YUV422P;
	case AV_PIX_FMT_YUVJ444P:
		return AV_PIX_FMT_YUV444P;
	case AV_PIX_FMT_YUVJ440P:
		return AV_PIX_FMT_YUV440P;
	default:
		return format;
	}
}

AVPixelFormat ImageFormatConverter::convert_input_format(InputPixelFormat format)
{
	switch (format)
	{
	case InputPixelFormat::YuvI420P:
		return AV_PIX_FMT_YUV420P;
	case InputPixelFormat::NV12:
		return AV_PIX_FMT_NV12;
	case InputPixelFormat::BGR24:
		return AV_PIX_FMT_BGR24;
	default:
		return AV_PIX_FMT_BGRA;
	}
}
