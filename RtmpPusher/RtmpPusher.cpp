// RtmpPusher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <cstring>
#include <iostream>
#include "Encoder.h"
#include <thread>
#include "Pusher.h"
#include "PushConfigCommon.h"
#include "Logger.h"
#include "DirectShowCameraCapture.h"
extern "C"
{
#include "libavutil/log.h"
}

void log_output(void* ptr, int level, const char* fmt, va_list vl);

int main()
{
	Logger::init("C:\\RTMPLog");
	av_log_set_callback(log_output);

	DirectShowCameraCapture dShowCapture;
	map<int, DeviceInfo> map;
	dShowCapture.get_camera_list(map);
	if (map.size() == 0)
	{
		cout << "Sorry, can'not find any camera on pc." << endl;
		system("Pause");
		return 0;
	}

	cout << "The camera list is below, please choose one:" << endl;
	for (std::map<int, DeviceInfo>::iterator it = map.begin(); it != map.end(); ++it)
	{
		std::map<int, DeviceInfo>::value_type item = (*it);
		cout << item.first << "." << item.second.friendlyName << endl;
	}

	int key;
	cin >> key;
	if (map.count(key) == 1)
	{
		DeviceInfo item = map[key];
		item.output();
		int configIndex;
		cin >> configIndex;
		CaptureFormat srcFormat = item.set_config(configIndex);
		int height = item.selectedConfig.height;
		int width = item.selectedConfig.width;
		int frameRate = item.selectedConfig.frameRate;
		PushConfig pushConfig("rtmp://192.168.6.98:1935/hls/stream", width, height, frameRate);
		Pusher* pusher = new Pusher();
		pusher->set_config(pushConfig);
		auto encodedCallBack = std::bind(&Pusher::push, pusher, std::placeholders::_1);
		Encoder* encoder = new Encoder(pushConfig, encodedCallBack, true);
		AVPixelFormat format = encoder->get_input_image_format();
		ImageCallBack imageCapturedCallBack = std::bind(&Encoder::encode_frame, encoder, std::placeholders::_1);
		CaptureInfo captureData(width, height, srcFormat, AV_PIX_FMT_NV12);
		dShowCapture.set_encode_callBack(imageCapturedCallBack, captureData);
		dShowCapture.capture(item);

		system("pause");
		delete pusher;
		delete encoder;
		Logger::write("APP is paused.");	
	}
}

void log_output(void* ptr, int level, const char* fmt, va_list vl) 
{
	char content[500];
	//只抓比warning 严重的log
	if (fmt && level <= AV_LOG_WARNING)
	{
		vsnprintf(content, sizeof(content), fmt, vl);
		Logger::write(content);
	}
}
