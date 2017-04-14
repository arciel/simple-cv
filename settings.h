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
	static settings& get();
	

	void reMOG2();

	// Camera and Source Capture Settings.
	float cap_scale = 0.50;
	float cap_height = 0;
	float cap_width = 0;
	float cap_ar = 0;

	// Size parameters for median blurring.
	cv::Size blurSize{ 3, 3 };

	// Strength parameters for Gaussian Blur.
	double sigmaX = 3;
	double sigmaY = 3;

	// Threshold Parameters for Canny's edge detection
	double canny1{ 10 }, canny2{ 70 }, cannyapp{ 3 }; 
	bool cannyblur = false; // Softens input to the canny algorithm. Eliminates flickering.
	
	// Color parameter for color isolation.
	cv::Scalar color_base = {};
	cv::Scalar color_lo = { 30,150,50 };
	cv::Scalar color_hi = { 255,255,180 };

	// Parameters for the MOG2 motion detector.
	cv::Ptr<cv::BackgroundSubtractorMOG2> mog2; //mog2 motion detector
	int mog2_history = 500; // Number of frames to consider
	bool mog2_shadows = true; // Detect shadows?
	bool mog2_soften = true; // Soften input to the mog2 detector? 
	bool mog2_thresh = 16; // Threshold parameter


	// Parameters for the contour detector.
	int ctr_minarea = 150; // Minimum area to consider 
	int ctr_maxarea = 500; // Maximum area to consider
	bool ctr_dilate = false; // Dilate mask before attempting to find contours?
	std::vector<std::vector<cv::Point>> ctr_list; // detected contours
	std::vector<std::vector<cv::Point>> ctr_filt; // considered contours


	// Make this a singleton class.
	settings(settings const&) = delete;
	settings(settings&&) = delete;
	settings& operator=(settings const&) = delete;
	settings& operator=(settings &&) = delete;
protected:
	settings();
};
