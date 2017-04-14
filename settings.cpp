#pragma once
#include "stdafx.h"
#include "settings.h"

settings::settings()
{
	mog2 = cv::createBackgroundSubtractorMOG2(500, 16, false);
}