#pragma once
#include <string>

struct DeviceConfig
{
	int width;
	int height;
	int frameRate;
	AM_MEDIA_TYPE* pMediaType;
	DeviceConfig(int w, int h, int f, AM_MEDIA_TYPE* mediaType) : width(w), height(h), frameRate(f), pMediaType(mediaType) {}
	DeviceConfig() {}
};

struct DeviceInfo
{
	std::string deviceId;
	std::string friendlyName;
	IBaseFilter* pDeviceFilter;
	IAMStreamConfig* pStreamConfig;
	std::vector<DeviceConfig> configs;
	DeviceConfig selectedConfig;

	DeviceInfo(std::string deviceId, std::string friendlyName) : deviceId(deviceId), friendlyName(friendlyName) {}
	DeviceInfo() {}

	void refresh(IMoniker* pMoniker, ICaptureGraphBuilder2* pCaptureGraphBuilder2)
	{
		pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pDeviceFilter);
		if (!pDeviceFilter)
		{
			return;
		}

		pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, pDeviceFilter,
			IID_IAMStreamConfig, (void**)&pStreamConfig);
		if (!pStreamConfig)
		{
			return;
		}

		int count = 0, size = 0;
		pStreamConfig->GetNumberOfCapabilities(&count, &size);
		if (size == sizeof(VIDEO_STREAM_CONFIG_CAPS))
		{
			for (int index = 0; index < count; index++)
			{
				VIDEO_STREAM_CONFIG_CAPS scc;
				AM_MEDIA_TYPE* pMediaType;
				pStreamConfig->GetStreamCaps(index, &pMediaType, (BYTE*)&scc);
				VIDEOINFOHEADER* phead = (VIDEOINFOHEADER*)pMediaType->pbFormat;
				if (phead->bmiHeader.biWidth > 0)
				{
					configs.push_back(DeviceConfig(phead->bmiHeader.biWidth, phead->bmiHeader.biHeight, 10000000 / phead->AvgTimePerFrame, pMediaType));
				}
			}
		}

		if (configs.size() > 0)
		{
			selectedConfig = configs[0];
		}
	}

	void output()
	{
		char temp[100];
		for (int i = 0; i < configs.size(); ++i)
		{
			DeviceConfig item = configs[i];

			if (item.pMediaType->subtype == MEDIASUBTYPE_MJPG)
			{
				sprintf_s(temp, 100, "%d.(%s)width:%d height:%d frame rate:%d \n", i, "MJPG", item.width, item.height, item.frameRate);
			}
			else
			{
				if (item.pMediaType->subtype == MEDIASUBTYPE_YUY2)
				{
					sprintf_s(temp, 100, "%d.(%s)width:%d height:%d frame rate:%d \n", i, "YUY2", item.width, item.height, item.frameRate);
				}

				sprintf_s(temp, 100, "%d.(%s)width:%d height:%d frame rate:%d \n", i, item.pMediaType->subtype, item.width, item.height, item.frameRate);
			}
			
			printf(temp);
		}
	}

	void set_config(int index)
	{
		selectedConfig = configs[index];
		pStreamConfig->SetFormat(selectedConfig.pMediaType);
	}
};

