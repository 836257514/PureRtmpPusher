#include "pch.h"
#include "Export.h"


void* __stdcall sd_create_instance(const char* streamUrl, int timeOut, InputPixelFormat targetPixelFormat, FrameReceived frameReceived)
{
	return new StreamReceiver(streamUrl, timeOut, targetPixelFormat, frameReceived);
}

StatusCode __stdcall sd_init_instance(void* instance)
{
	return static_cast<StreamReceiver*>(instance)->init();
}

void __stdcall sd_receive(void* instance)
{
	static_cast<StreamReceiver*>(instance)->receive();
}