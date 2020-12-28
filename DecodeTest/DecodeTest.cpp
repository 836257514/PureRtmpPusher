// DecodeTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Export.h"


int main()
{
    void* instance = sd_create_instance("rtmp://192.168.6.98:1935/hls/stream", 3000, ConvertPixelFormat::YuvI420P);
    StatusCode statusCode = sd_init_instance(instance);
    if (statusCode == StatusCode::Success)
    {
        sd_receive(instance);
    }

    system("pause");
}
