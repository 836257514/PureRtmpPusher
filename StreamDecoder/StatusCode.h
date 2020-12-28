#pragma once

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

