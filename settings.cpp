#pragma once
#include "stdafx.h"
#include "settings.h"

settings::settings()
{
	mog2 = cv::createBackgroundSubtractorMOG2(500, 16, mog2_shadows);
}

settings& settings::get()
{
	static settings instance;
	return instance;
}

// Re-initializes the MOG2 detector.
void settings::reMOG2()
{
	mog2->clear();
	mog2->setHistory(mog2_history);
	mog2->setDetectShadows(mog2_shadows);
	mog2->setVarThreshold(mog2_thresh);
}