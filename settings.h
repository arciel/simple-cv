#pragma once
#include "stdafx.h"

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include "imgui.h"

struct settings
{
	static settings& get()
	{
		static settings instance;
		return instance;
	}
	//Camera and Source Capture Settings.
	float cap_scale = 0.25;
	float cap_height = 0;
	float cap_width = 0;
	float cap_ar = 0;

	cv::Size blurSize{ 3, 3 }; //Size for blurring --> Single Slider
	
	double canny1{ 10 }, canny2{ 70 }, cannyapp{ 3 }; // parameters for Canny --> Sliders x3
	bool cannyblur = false;
	
	cv::Scalar color_lo{ 30,150,50 }, color_hi{ 255,255,180 }; //parameters for color isolation (pipeline_1color) --> color wheel

	cv::Ptr<cv::BackgroundSubtractorMOG2> mog2; //mog2 motion detector
	std::vector<std::vector<cv::Point>> contours; //contours found this frame.
	int contour_granularity = 150; //for filtering out false positives
	std::vector<std::vector<cv::Point>> big_contours; //filtered contours

	settings(settings const&) = delete;
	settings(settings&&) = delete;
	settings& operator=(settings const&) = delete;
	settings& operator=(settings &&) = delete;
protected:
	settings();
};
