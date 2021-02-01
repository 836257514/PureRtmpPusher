// DecodeTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Export.h"
#include <VideoFrame.h>

void frame_received(VideoFrame&);

int main()
{
    void* instance = sd_create_instance("rtmp://liveplay.fis.plus/live/368e85ad67684279bc89037f09e2f773", 3000000, InputPixelFormat::NV12, frame_received);
    StatusCode statusCode = sd_init_instance(instance);
    if (statusCode == StatusCode::Success)
    {
        sd_receive(instance);
    }

    system("pause");
}

void frame_received(VideoFrame& frame)
{

}
