// RtmpPusher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ImageCapturer.h"
#include "Encoder.h"
#include <thread>
#include "Pusher.h"
#include "PushConfigCommon.h"

PushConfig get_push_config();
VideoConfig get_video_config(PushConfig& pushConfig, std::string& deviceName);

int main()
{
	cout << "input device name:" << endl;
	std::string deviceName;
	getline(cin, deviceName);
	PushConfig pushConfig = get_push_config();
	VideoConfig videoConfig = get_video_config(pushConfig, deviceName);

	Pusher* pusher = new Pusher();
	pusher->set_config(pushConfig);
	auto encodedCallBack = std::bind(&Pusher::push, pusher, std::placeholders::_1);
	Encoder* encoder = new Encoder(pushConfig, encodedCallBack);
	AVPixelFormat format = encoder->get_input_image_format();

	auto imageCapturedCallBack = std::bind(&Encoder::encode_frame,
		encoder, std::placeholders::_1);
	ImageCapturer* capture = new ImageCapturer(imageCapturedCallBack, format);

	bool readyToUse = capture->open_camera(videoConfig);
	if (readyToUse)
	{
		std::thread t(&ImageCapturer::read, capture);
		t.join();
	}

	delete pusher;
	delete encoder;
	delete capture;
}

PushConfig get_push_config()
{
	PushConfig pushConfig = { "rtmp://livepush.fis.plus/live/ee9bb35a72e54429a2a80e3659b2e59d?txSecret=dbb0d611a0fa993cac06b76aafff9a95&txTime=5FFDC780", 1280, 720, 30 };
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
	config.inputUrl = inputUrl.c_str();
	cout << "the input device name is :" << config.inputUrl << endl;
	config.map["video_size"] = std::to_string(pushConfig.width) + "x" + std::to_string(pushConfig.height);
	config.map["framerate"] = std::to_string(pushConfig.frameRate);
	config.map["vcodec"] = "mjpeg";
	return config;
}

