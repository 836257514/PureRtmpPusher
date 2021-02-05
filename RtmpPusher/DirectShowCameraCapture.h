#pragma once
#include <iostream>
#include <dshow.h>
#include "Logger.h"
#include <map>

using namespace std;

class DirectShowCameraCapture
{
public:
    DirectShowCameraCapture();
    ~DirectShowCameraCapture();
    HRESULT get_camera_list(map<int, string>&);
private:
    HRESULT enum_device(map<int, string>&, IEnumMoniker*);
};

