#include "DirectShowCameraCapture.h"

DirectShowCameraCapture::DirectShowCameraCapture() :
	m_pEnumMoniker(nullptr),
	m_pFrameGrabberFilter(nullptr),
	m_pGraph(nullptr),
	m_pCaptureGraphBuilder2(nullptr)
{
	HRESULT hr = CoInitialize(nullptr);
	if (FAILED(hr)) {
		Logger::write("Can not init COM.");
	}

	ICreateDevEnum* createDevEnum;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (void**)&createDevEnum);
	if (FAILED(hr)) {
		Logger::write("Can not create COM instance");
	}

	hr = createDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &m_pEnumMoniker, 0);
	if (FAILED(hr))
	{
		Logger::write("Create enum moniker failed");
	}

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (void**)&m_pGraph);
	if (FAILED(hr))
	{
		Logger::write("Create filter graph failed");
	}

	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (void**)&m_pCaptureGraphBuilder2);
	if (FAILED(hr))
	{
		Logger::write("Create capture graph builder failed");
	}

	m_pCaptureGraphBuilder2->SetFiltergraph(m_pGraph);
	hr = m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pMediaControl);
	if (FAILED(hr))
	{
		Logger::write("Query media control failed");
	}

	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pFrameGrabberFilter));
	if (FAILED(hr))
	{
		Logger::write("Create sample grabber failed");
	}

	m_pGraph->AddFilter(m_pFrameGrabberFilter, L"Grabber Filter");
	ISampleGrabber* gGrabber;
	hr = m_pFrameGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&gGrabber);
	if (gGrabber)
	{
		gGrabber->SetCallback(&m_sampleGrabberCallBack, (int)GrabberCallBackCategory::BufferCB);
		gGrabber->SetOneShot(false);
		gGrabber->SetBufferSamples(true);
	}
}

DirectShowCameraCapture::~DirectShowCameraCapture()
{
	if (m_pMediaControl)
	{
		m_pMediaControl->Stop();
		m_pMediaControl->Release();
	}

	if (m_pEnumMoniker)
	{
		m_pEnumMoniker->Release();
	}

	if (m_pFrameGrabberFilter)
	{
		m_pFrameGrabberFilter->Release();
	}

	if (m_pCaptureGraphBuilder2)
	{
		m_pCaptureGraphBuilder2->Release();
	}

	if (m_pGraph)
	{
		m_pGraph->Release();
	}

	CoUninitialize();
}

HRESULT DirectShowCameraCapture::get_camera_list(map<int, DeviceInfo>& cameraMap)
{
	m_pEnumMoniker->Reset();
	char displayName[1024] = { 0 };
	char friendlyName[256] = { 0 };
	unsigned long fetched = 0;
	IMoniker* pMoniker = nullptr;
	HRESULT result = E_FAIL; //fail;
	int index = 0;
	while (m_pEnumMoniker->Next(1, &pMoniker, &fetched) == S_OK)
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
					string key(displayName, displayName + strlen(displayName));
					string value(friendlyName, friendlyName + strlen(friendlyName));
					DeviceInfo deviceInfo(key, value);
					deviceInfo.refresh(pMoniker, m_pCaptureGraphBuilder2);
					cameraMap.insert(make_pair(index, deviceInfo));
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

HRESULT DirectShowCameraCapture::capture(DeviceInfo& deviceInfo)
{
	HRESULT hr = E_FAIL;
	ISampleGrabber* gGrabber;
	hr = m_pFrameGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&gGrabber);
	AM_MEDIA_TYPE mediaType(*(deviceInfo.selectedConfig.pMediaType));
	mediaType.subtype = directshow_format_convert(deviceInfo.selectedConfig.pMediaType->subtype);
	gGrabber->SetMediaType(&mediaType);
	m_pGraph->AddFilter(deviceInfo.pDeviceFilter, L"Device Filter");
	hr = m_pCaptureGraphBuilder2->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, deviceInfo.pDeviceFilter, NULL, m_pFrameGrabberFilter);
	if (SUCCEEDED(hr) && m_pMediaControl)
		hr = m_pMediaControl->Run();
	return hr;
}

GUID DirectShowCameraCapture::directshow_format_convert(GUID subtype)
{
	if (subtype == MEDIASUBTYPE_MJPG)
	{
		return MEDIASUBTYPE_RGB24;
	}

	return subtype;
}

void DirectShowCameraCapture::set_encode_callBack(ImageCallBack& imageCaptureCB, CaptureInfo& cameraData)
{
	m_sampleGrabberCallBack.SetCallback(imageCaptureCB, cameraData);
}