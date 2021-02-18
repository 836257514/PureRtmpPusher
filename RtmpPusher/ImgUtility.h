#pragma once
extern "C"
{
#include <libavutil\frame.h>
#include <libavdevice/avdevice.h>
}

#include "CImg.h"
using namespace cimg_library_suffixed;

class ImgUtility
{
public:
	static void save_image(AVFrame* rgbFrame, const char* savePath)
	{
		CImg<uint8_t> img(rgbFrame->width, rgbFrame->height, 1, 3);
		int index = 0;
		for (int row = 0; row < rgbFrame->height; ++row)
		{
			for (int col = 0; col < rgbFrame->width; ++col)
			{
				*(img.data(col, row, 0, 0)) = rgbFrame->data[0][index];
				*(img.data(col, row, 0, 1)) = rgbFrame->data[0][++index];
				*(img.data(col, row, 0, 2)) = rgbFrame->data[0][++index];
				index++;
			}
		}

		//const char* const filename = "C:\\1.bmp";
		img.save_bmp(savePath);
	}

	static AVPixelFormat convert_deprecated_format(AVPixelFormat format)
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

	static AVPacket* create_packet()
	{
		AVPacket* avPacket = av_packet_alloc();
		if (avPacket)
		{
			av_init_packet(avPacket);
		}
		
		return avPacket;
	}


	static void rgb_to_yuv420p(unsigned  char* yuv420p, unsigned char* rgb, int width, int height)
	{
		if (yuv420p == NULL || rgb == NULL)
			return;
		int frameSize = width * height;
		int chromaSize = frameSize / 4;

		int yIndex = 0;
		int uIndex = frameSize;
		int vIndex = frameSize + chromaSize;

		int R, G, B, Y, U, V;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				B = rgb[(i * width + j) * 3 + 0];
				G = rgb[(i * width + j) * 3 + 1];
				R = rgb[(i * width + j) * 3 + 2];

				//RGB to YUV
				Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
				U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
				V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;

				yuv420p[yIndex++] = (unsigned char)((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
				if (i % 2 == 0 && j % 2 == 0)
				{
					yuv420p[uIndex++] = (unsigned char)((U < 0) ? 0 : ((U > 255) ? 255 : U));
					yuv420p[vIndex++] = (unsigned char)((V < 0) ? 0 : ((V > 255) ? 255 : V));
				}
			}
		}
	}
};

