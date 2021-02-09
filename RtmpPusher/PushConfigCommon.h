#pragma once
#include <string>

struct PushConfig
{
	std::string url;
	int width;
	int height;
	int frameRate;

	PushConfig(std::string u, int w, int h, int f):url(u), width(w), height(h), frameRate(f) {}
};
