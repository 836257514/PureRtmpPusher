// RtmpPusher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <cstring>
#include <iostream>
#include "ImageCapturer.h"
#include "Encoder.h"
#include <thread>
#include "Pusher.h"
#include "PushConfigCommon.h"
#include "Logger.h"

extern "C"
{
#include "libavutil/log.h"
}

void log_output(void* ptr, int level, const char* fmt, va_list vl);
PushConfig get_push_config();
VideoConfig get_video_config(PushConfig& pushConfig, std::string& deviceName);

int main()
{
	Logger::init("C:\\RTMPLog");
	av_log_set_callback(log_output); 
	cout << "input device name:" << endl;
	std::string deviceName;
	getline(cin, deviceName);
	PushConfig pushConfig = get_push_config();
	VideoConfig videoConfig = get_video_config(pushConfig, deviceName);

	Pusher* pusher = new Pusher();
	pusher->set_config(pushConfig);
	auto encodedCallBack = std::bind(&Pusher::push, pusher, std::placeholders::_1);
	Encoder* encoder = new Encoder(pushConfig, encodedCallBack, true);
	AVPixelFormat format = encoder->get_input_image_format();

	auto imageCapturedCallBack = std::bind(&Encoder::encode_frame,
		encoder, std::placeholders::_1);
	ImageCapturer* capture = new ImageCapturer(imageCapturedCallBack, format);

	bool readyToUse = capture->open_camera(videoConfig);
	if (readyToUse)
	{
		std::thread t1(&ImageCapturer::read, capture);
		t1.join();
	}

	delete pusher;
	delete encoder;
	delete capture;
	Logger::write("APP is paused.");
	system("pause");
}

PushConfig get_push_config()
{
	PushConfig pushConfig = { "rtmp://192.168.6.98:1935/hls/stream", 1280, 720, 30 };
	cout << "press 1 to push 1080P 30fps, press 2 to push 720p 30fps, press 3 to push 480p 30fps:" << endl;
	int choose = 0;
	cin >> choose;
	if (choose == 1)
	{
		pushConfig.width = 1920;
		pushConfig.height = 1080;
	}

	if (choose == 3)
	{
		pushConfig.width = 640;
		pushConfig.height = 480;
	}

	return pushConfig;
}

VideoConfig get_video_config(PushConfig& pushConfig, std::string& deviceName)
{
	VideoConfig config;
	std::string inputUrl = "video=" + deviceName;
	unsigned int length = inputUrl.length() + 1;
	config.inputUrl = new char[length]();
	strcpy_s(config.inputUrl, length, inputUrl.c_str());
	cout << "the input device name is :" << config.inputUrl << endl;
	config.map["video_size"] = std::to_string(pushConfig.width) + "x" + std::to_string(pushConfig.height);
	config.map["framerate"] = std::to_string(pushConfig.frameRate);
	config.map["vcodec"] = "mjpeg";
	return config;
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
