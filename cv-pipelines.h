#pragma once


#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/background_segm.hpp>

auto pipeline_grayscale(const cv::Mat& m)->cv::Mat;
auto pipeline_invert(const cv::Mat& m)->cv::Mat;
auto pipeline_canny(const cv::Mat& m)->cv::Mat;
auto pipeline_blur(const cv::Mat& m)->cv::Mat;
auto pipeline_1color(const cv::Mat& m)->cv::Mat;
auto pipeline_toon(const cv::Mat& m)->cv::Mat;

auto pipeline_colorMOG(const cv::Mat& m)->cv::Mat;
auto pipeline_contourMOG(const cv::Mat& m)->cv::Mat;
auto pipeline_boxMOG(const cv::Mat& m)->cv::Mat;