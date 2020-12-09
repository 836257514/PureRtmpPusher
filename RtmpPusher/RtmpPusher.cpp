// RtmpPusher.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ImageCapturer.h"
#include "Encoder.h"
#include <thread>
#include "Pusher.h"


int main()
{
    PushConfig pushConfig = {"rtmp://192.168.6.98:1935/hls/stream", 1280, 720, 30};
    Pusher* pusher = new Pusher();
    pusher->set_config(pushConfig);

    auto encodedCallBack = std::bind(&Pusher::push, pusher, std::placeholders::_1);
    Encoder* encoder = new Encoder(1280, 720, 30, encodedCallBack);

    auto imageCapturedCallBack = std::bind(&Encoder::encode_frame,
        encoder, std::placeholders::_1);
    ImageCapturer* capture = new ImageCapturer(imageCapturedCallBack);
    VideoConfig config;
    config.inputUrl = "video=Rxsx Video";
    config.map["video_size"] = "1280x720";
    config.map["framerate"] = "30";
    config.map["vcodec"] = "mjpeg";
    bool readyToUse = capture->open_camera(config);
    if (readyToUse)
    {
        std::thread t(&ImageCapturer::read, capture);
        t.join();
    }   

    delete pusher;
    delete encoder;
    delete capture;
}

