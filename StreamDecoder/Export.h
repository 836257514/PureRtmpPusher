#pragma once
#include "StatusCode.h"
#include "StreamReceiver.h"
#include <libavdevice\avdevice.h>

extern "C"
{
	_declspec(dllexport) void* __stdcall sd_create_instance(const char* streamUrl, int timeOut, InputPixelFormat targetPixelFormat, FrameReceived frameReceived);

	_declspec(dllexport) void __stdcall sd_destroy_instance(void* instance);

	_declspec(dllexport) StatusCode __stdcall sd_init_instance(void* instance, bool enableHardwareDecode = false);

	_declspec(dllexport) void __stdcall sd_keep_receive(void* instance);
}

