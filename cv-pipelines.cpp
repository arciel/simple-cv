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

#define SAMPLE(x) "C:\\dhoomketu\\samples\\" ## x

#define arg SAMPLE("terrace1-c3.avi")

int bmain(int argc, char *argv[])
{
	cv::VideoCapture video(arg);
	cv::Mat prev_frame, cur_frame, next_frame;
	bool success;
	video >> prev_frame >> cur_frame;

	while (true)
	{
		video >> next_frame;

		cv::Mat pc;
		cv::absdiff(prev_frame, cur_frame, pc);
		cv::Mat cn;
		cv::absdiff(cur_frame, next_frame, cn);

		cv::imshow("pc", pc);
		cv::imshow("cn", cn);

		

		//cv::Mat frame_diff = motion_frame_diff(prev_frame, cur_frame, next_frame);

		//cv::imshow("prev_frame", prev_frame);
		//cv::imshow("cur_frame", cur_frame);
		//cv::imshow("next_frame", next_frame);

		//cv::imshow("frame_diff", frame_diff);

		prev_frame = cur_frame;
		cur_frame = next_frame;

		int key = cv::waitKey(1);
		if (key == 'q')
			break;
	}
	cv::destroyAllWindows();
	return 0;
}


/*
auto main(int argc, char *argv[]) -> int
{
	cv::VideoCapture cap {arg};
	cap.re
	cv::Mat frame, fin;
	int tc = 0;
	while ((tc = cv::waitKey(60)) != 'q')
	{
		cap >> frame;
		if (frame.empty()) break;
		fin = pipeline_boxMOG(frame);
		cv::imshow("Frame", frame);
		cv::imshow("Result", fin);
		fin = cv::Scalar(0);
	}
	cv::destroyAllWindows();
	return 0;
}


*/

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
	cv::blur(i, r, settings::get().blurSize);
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
	cv::Mat m, r;
	settings::get().mog2->apply(i, m);
	cv::bitwise_and(i, i, r, m);
	return r;
}

auto pipeline_contourMOG(const cv::Mat& i) -> cv::Mat
{
	cv::Mat delta, delta2, bi, mask, r;
	settings::get().contours.clear();
	settings::get().big_contours.clear();
	cv::GaussianBlur(i, bi, cv::Size(0, 0), 1, 1);
	settings::get().mog2->apply(bi, delta);
	delta2 = delta.clone();
	cv::findContours(delta2, settings::get().contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	settings::get().big_contours.reserve(settings::get().contours.size());
	auto it = std::back_inserter(settings::get().big_contours);
	std::copy_if(settings::get().contours.begin(), settings::get().contours.end(), it, [](const auto& c) { return cv::contourArea(c) > settings::get().contour_granularity; });
	r = i.clone();
	cv::drawContours(r, settings::get().big_contours, -1, cv::Scalar(0, 0, 255), 5);
	return r;
}

auto pipeline_boxMOG(const cv::Mat& i) -> cv::Mat
{
	cv::Mat delta, delta2, bi, mask, r;
	settings::get().contours.clear();
	settings::get().big_contours.clear();
	cv::GaussianBlur(i, bi, cv::Size(0, 0), 1, 1);
	settings::get().mog2->apply(bi, delta);
	cv::dilate(mask, mask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)));
	delta2 = delta.clone();
	cv::findContours(delta2, settings::get().contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
	r = i.clone();
	for (const auto& i : settings::get().contours)
	{
		if (cv::contourArea(i) > settings::get().contour_granularity)
		{
			cv::Rect rct = cv::boundingRect(i);
			cv::rectangle(r, rct, cv::Scalar(0, 0, 255), 3);
		}
	}
	return r;
}