#pragma once
#include <libavutil\pixfmt.h>
#include "StatusCode.h"
class ImageFormatConverter
{
public:
	static AVPixelFormat convert_deprecated_format(AVPixelFormat format);
	static AVPixelFormat convert_input_format(InputPixelFormat format);
};

