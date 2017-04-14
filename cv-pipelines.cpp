// simplecv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

#include <SDL.h>

#include "settings.h"

auto pipeline_canny(const cv::Mat& i) -> cv::Mat
{
	cv::Mat bi = i, r;
	if(settings::get().cannyblur) cv::blur(i, bi, settings::get().blurSize);
	cv::Canny(bi, r, settings::get().canny1, settings::get().canny2, settings::get().cannyapp);
	cv::cvtColor(r, r, cv::COLOR_GRAY2BGR);
	return r;
}

auto pipeline_blur(const cv::Mat& i) -> cv::Mat
{
	cv::Mat r;
	//cv::blur(i, r, settings::get().blurSize);
	cv::GaussianBlur(i, r, cv::Size(0, 0), settings::get().sigmaX, settings::get().sigmaY);
	return r;
}

auto pipeline_grayscale(const cv::Mat& i) -> cv::Mat
{
	cv::Mat r;
	cv::cvtColor(i, r, cv::COLOR_BGR2GRAY);
	cv::cvtColor(r, r, cv::COLOR_GRAY2BGR); //stupid opengl GL_RED;
	return r;
}

auto pipeline_invert(const cv::Mat& i) -> cv::Mat
{
	cv::Mat r;
	cv::bitwise_not(i, r);
	return r;
}

auto pipeline_1color(const cv::Mat& i) -> cv::Mat
{
	cv::Mat r, ihsv, mask;
	cv::cvtColor(i, ihsv, cv::COLOR_BGR2HSV);
	cv::inRange(ihsv, settings::get().color_lo, settings::get().color_hi, mask);
	cv::bitwise_and(i, i, r, mask);
	return r;
}


auto pipeline_colorMOG(const cv::Mat& i) -> cv::Mat
{
	cv::Mat igs, mask, ret;
	cv::cvtColor(i, igs, cv::COLOR_BGR2GRAY);
	if (settings::get().mog2_soften) cv::blur(igs, igs, settings::get().blurSize);
	settings::get().mog2->apply(igs, mask);
	cv::cvtColor(igs, igs, cv::COLOR_GRAY2BGR);
	cv::bitwise_and(i, i, ret, mask);
	cv::bitwise_not(mask, mask);
	cv::bitwise_and(igs, igs, ret, mask);
	return ret;
}

auto pipeline_contourMOG(const cv::Mat& i) -> cv::Mat
{
	cv::Mat igs, mask, mask2, ret;
	//settings::get().contours.clear();
	settings::get().ctr_list.clear();
	//settings::get().big_contours.clear();
	settings::get().ctr_filt.clear();
	//cv::GaussianBlur(i, bi, cv::Size(0, 0), 1, 1);
	cv::cvtColor(i, igs, cv::COLOR_BGR2GRAY);
	if (settings::get().mog2_soften) cv::blur(igs, igs, settings::get().blurSize);
	//settings::get().mog2->apply(bi, delta);
	settings::get().mog2->apply(igs, mask);
	//delta2 = delta.clone();
	mask2 = mask.clone();
	//cv::findContours(delta2, settings::get().contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	if (settings::get().ctr_dilate) cv::dilate(mask2, mask2, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	cv::findContours(mask2, settings::get().ctr_list, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	settings::get().ctr_filt.reserve(settings::get().ctr_list.size());
	auto it = std::back_inserter(settings::get().ctr_filt);
	std::copy_if(settings::get().ctr_list.begin(), settings::get().ctr_list.end(), it, [](const auto& c)
	{ 
		return cv::contourArea(c) > settings::get().ctr_minarea && cv::contourArea(c) < settings::get().ctr_maxarea;
	});
	ret = i.clone();
	cv::drawContours(ret, settings::get().ctr_filt, -1, cv::Scalar(0, 0, 255), 5);
	return ret;
}

auto pipeline_boxMOG(const cv::Mat& i) -> cv::Mat
{
	return pipeline_blur(i);
}