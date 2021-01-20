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

	enum class InputPixelFormat
	{
		YuvI420P,
		NV12,
		BGR24,
		BGRA,
	};
}

