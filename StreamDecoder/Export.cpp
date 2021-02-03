#include "pch.h"
#include "Export.h"


void* __stdcall sd_create_instance(const char* streamUrl, int timeOut, InputPixelFormat targetPixelFormat, FrameReceived frameReceived)
{
	return new StreamReceiver(streamUrl, timeOut, targetPixelFormat, frameReceived);
}

void delete_instance(StreamReceiver* instance)
{
	delete instance;
}

void __stdcall sd_destroy_instance(void* instance)
{
	auto pInstance = static_cast<StreamReceiver*>(instance);
	if (pInstance != NULL)
	{
		std::thread(delete_instance, pInstance).detach();
	}
}

StatusCode __stdcall sd_init_instance(void* instance, bool enableHardwareDecode)
{
	return static_cast<StreamReceiver*>(instance)->init(enableHardwareDecode);
}

void __stdcall sd_keep_receive(void* instance)
{
	static_cast<StreamReceiver*>(instance)->keep_receive();
}