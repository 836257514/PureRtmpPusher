#include "pch.h"
#include "Export.h"


void* __stdcall sd_create_instance(const char* streamUrl, int timeOut, InputPixelFormat targetPixelFormat, FrameReceived frameReceived)
{
	return new StreamReceiver(streamUrl, timeOut, targetPixelFormat, frameReceived);
}

void __stdcall sd_destroy_instance(void* instance)
{
	delete (static_cast<StreamReceiver*>(instance));
}

StatusCode __stdcall sd_init_instance(void* instance, bool enableHardwareDecode)
{
	return static_cast<StreamReceiver*>(instance)->init(enableHardwareDecode);
}

void __stdcall sd_receive(void* instance)
{
	static_cast<StreamReceiver*>(instance)->receive();
}