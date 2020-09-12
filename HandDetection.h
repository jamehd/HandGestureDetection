#pragma once

#include "Frame.h"

#include <memory>



class HandDetection
{
public:
        HandDetection(Frame& f);

	void initBkg();
	void processing();
	void drawResult();

protected:
	bool segment(
		const cv::Mat& image,
		cv::Mat bg,
		cv::Mat& thresholded,
		std::vector<std::vector<cv::Point>>& segmented,
		const int threshold_value = 25);

	int getMaxAreaContourId(
		const std::vector<std::vector<cv::Point>>& contours);

	GESTURE detectGesture(
		cv::Mat& thresholded,
		std::vector<std::vector<cv::Point>>& segmented,
		std::vector<std::vector<cv::Point>>& hulls,
		std::vector<cv::Vec4i>& defect,
		cv::Point& centerPoint,
		double& radius);

private:
        Frame& frame_;
};

