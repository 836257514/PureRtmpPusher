#pragma once
extern "C"
{
	enum class StatusCode
	{
		Success,
		OpenInputFail,
		FindStreamInfoFail,
		CanNotFindVideoStream,
		CanNotFindDecoder,
		CodecParameterToContextFail,
		DecoderToContextFail
	};

	enum class ConvertPixelFormat
	{
		YuvI420P,
		BGR24,
		BGRA
	};
}

