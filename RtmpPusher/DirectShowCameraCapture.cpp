#include "DirectShowCameraCapture.h"

DirectShowCameraCapture::DirectShowCameraCapture()
{
}

DirectShowCameraCapture::~DirectShowCameraCapture()
{
    CoUninitialize();
}

HRESULT DirectShowCameraCapture::get_camera_list(map<int, string>& cameraMap)
{
    HRESULT result = CoInitialize(nullptr);
    if (FAILED(result)) {
        Logger::write("Can not init COM.");
        return result;
    }

    ICreateDevEnum* createDevEnum;
    result = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void**)&createDevEnum);
    if (FAILED(result)) {
        Logger::write("Can not create COM instance");
        return result;
    }

    IEnumMoniker* enumMoniker;
    result = createDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMoniker, 0);
    if (FAILED(result))
    {
        Logger::write("Find camera failed");
        return result;
    }

    result = enum_device(cameraMap, enumMoniker);
    if (!enumMoniker)
    {
        enumMoniker->Release();
    }
    
    return result;
}

HRESULT DirectShowCameraCapture::enum_device(map<int, string>& cameraMap, IEnumMoniker* enumMoniker)
{

    char displayName[1024] = { 0 };
    char friendlyName[256] = { 0 };
    unsigned long fetched = 0;
    IMoniker* pMoniker = nullptr;
    HRESULT result = E_FAIL; //fail;
    int index = 0;
    while (enumMoniker->Next(1, &pMoniker, &fetched) == S_OK)
    {
        if (!pMoniker)
        {
            continue;
        }

        WCHAR* wDisplayName = nullptr;
        IPropertyBag* pPropertyBag = nullptr;
        VARIANT name;
        result = pMoniker->GetDisplayName(NULL, NULL, &wDisplayName);
        if (SUCCEEDED(result))
        {
            WideCharToMultiByte(CP_ACP, 0, wDisplayName, -1, displayName, 1024, "", NULL);
            CoTaskMemFree(wDisplayName);
            result = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pPropertyBag);
            if (SUCCEEDED(result))
            {
                name.vt = VT_BSTR;
                result = pPropertyBag->Read(L"FriendlyName", &name, NULL);
                if (result == S_OK)
                {
                    WideCharToMultiByte(CP_ACP, 0, name.bstrVal, -1, friendlyName, 256, NULL, NULL);
                    string value(friendlyName, friendlyName + strlen(friendlyName));
                    cameraMap.insert(make_pair(index, value));
                }
            }
        }

        if (pPropertyBag)
        {
            pPropertyBag->Release();
            pPropertyBag = nullptr;
        }
        index++;
    }

    if (pMoniker)
    {
        pMoniker->Release();
    }

    return result;
}
