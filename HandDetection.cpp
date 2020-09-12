#include "HandDetection.h"

#include <iostream>

#include <math.h>

const double accumWeight = 0.5;
const int max_BINARY_value = 255;
const double maxDistOfFinger = 10.0;

static double euclideanDist(const cv::Point& p, const cv::Point& q) {
	cv::Point diff = p - q;
	return cv::sqrt(diff.x * diff.x + diff.y * diff.y);
}

static void reduceNoise(std::vector<std::shared_ptr<cv::Point>> defects)
{
	for (size_t i = 0; i < defects.size() - 1; i++)
	{
		for (size_t j = i + 1; j < defects.size(); j++)
		{
			auto& pointI = defects[i];
			auto& pointJ = defects[j];
			if (cv::norm(*pointI - *pointJ) < maxDistOfFinger)
			{
				pointI->x = pointJ->x = (pointI->x + pointJ->x) / 2;
				pointI->y = pointJ->y = (pointI->y + pointJ->y) / 2;
			}
		}
	}
}

static double cosFarPoint(const cv::Point& startP, const cv::Point& endP, const cv::Point& farP)
{
	auto FS = euclideanDist(farP, startP);
	auto SE = euclideanDist(startP, endP);
	auto FE = euclideanDist(farP, endP);
	return ((FS * FS + FE * FE - SE * SE) / (2 * FS * FE));
}

HandDetection::HandDetection(Frame& f)
	: frame_(f)
{
}

void HandDetection::initBkg()
{
	if (frame_.count < 30)
	{
		if (frame_.bkg.empty())
		{
			frame_.bkg = frame_.handArea.clone();
			frame_.bkg.convertTo(frame_.bkg, CV_32FC1);
			return;
		}
		cv::accumulateWeighted(frame_.handArea, frame_.bkg, accumWeight);
		if (1 == frame_.count)
		{
			std::cout << "[STATUS] Please wait! Calibrating..." << std::endl;
		}
		else if (29 == frame_.count)
		{
			std::cout << "[STATUS] Calibration Successfull..." << std::endl;
		}
	}
}

int HandDetection::getMaxAreaContourId(const std::vector<std::vector<cv::Point>>& contours)
{
	double maxArea = 0;
	int maxAreaContourId = -1;
	for (size_t j = 0; j < contours.size(); j++) {
		double newArea = cv::contourArea(contours.at(j));
		if (newArea > maxArea) {
			maxArea = newArea;
			maxAreaContourId = int(j);
		}
	}
	return maxAreaContourId;
}

bool HandDetection::segment(
	const cv::Mat& image,
	cv::Mat bg,
	cv::Mat& thresholded,
	std::vector<std::vector<cv::Point>>& segmented,
	const int threshold_value)
{
	cv::Mat diff;
	bg.convertTo(bg, CV_8UC1);
	cv::absdiff(bg, image, diff);

	if (cv::countNonZero(diff) < 1)
	{
		return false;
	}

	cv::threshold(diff, thresholded, threshold_value, max_BINARY_value, cv::THRESH_BINARY);

	cv::Mat canny_output = thresholded.clone();
	std::vector<std::vector<cv::Point>> contours;

	cv::findContours(canny_output, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	if (contours.empty())
	{
		//std::cout << "Contour is empty\n";
		return false;
	}
	else
	{
		try
		{
			segmented.push_back(contours.at(getMaxAreaContourId(contours)));
		}
		catch (const std::out_of_range&)
		{
			//std::cout << "out_of_range\n";
			return false;
		}
		return true;
	}
}
void nothing(void)
{

}

GESTURE HandDetection::detectGesture(
	cv::Mat& thresholded,
	std::vector<std::vector<cv::Point>>& segmented,
	std::vector<std::vector<cv::Point>>& hulls,
	std::vector<cv::Vec4i>& defect,
	cv::Point& centerPoint,
	double& radius)
{
	int count = 0;

	std::vector<cv::Point> hullP;
	std::vector<int> hullI;
	cv::convexHull(segmented[0], hullP, false, true);
	cv::convexHull(segmented[0], hullI, false, false);
	hulls.push_back(hullP);

	cv::convexityDefects(segmented[0], hullI, defect);

	auto segmentArea = cv::contourArea(segmented[0]);
	auto hullArea = cv::contourArea(hullP);
	auto areaRatio = 100 * (hullArea - segmentArea) / segmentArea;

	for (const auto& i : frame_.defect)
	{
		auto startPoint = frame_.segmented[0][i[0]] + originROI;
		auto endPoint = frame_.segmented[0][i[1]] + originROI;
		auto farPoint = frame_.segmented[0][i[2]] + originROI;
		double depth = i[3] / 256;
		auto FS = euclideanDist(farPoint, startPoint);
		auto SE = euclideanDist(startPoint, endPoint);
		auto FE = euclideanDist(farPoint, endPoint);
		auto angleFar = ::acos((FS * FS + FE * FE - SE * SE) / (2 * FS * FE)) * 180.0 / CV_PI;	// Goc FarPoint
		//auto p = (FS + SE + FE) / 2;															// 1/2 Chu vi
		//auto S = ::sqrt(p * (p - FS) * (p - SE) * (p - FE));									// Dien tich tam giac
		//auto d = (2 * S) / SE;																// Depth
		if (angleFar <= 90 && depth > 20)
		{
			count++;
		}
	}
	count++;
//	std::cout << segmentArea << " - " << areaRatio << std::endl;

	if (1 == count)
	{
		if (segmentArea < 10000)
		{
			return GESTURE::NO_DATA;
		}
		if (areaRatio < 10)
		{
			return GESTURE::ZERO;
		}
		//if (areaRatio < 30)
		//{
		//	return GESTURE::BEST_OF_LUCK;
		//}
		return GESTURE::ONE;
	}
	else if (2 == count)
	{
		return GESTURE::TWO;
	}
	else if (3 == count)
	{
		//if (areaRatio < 20)
		//{
		return GESTURE::THREE;
		//}
		//return GESTURE::OKE;
	}
	else if (4 == count)
	{
		return GESTURE::FOUR;
	}
	else if (5 == count)
	{
		return GESTURE::FIVE;
	}
	else
	{
		return GESTURE::INVALID;
	}
}

void HandDetection::processing()
{
	auto res = segment(frame_.handArea, frame_.bkg, frame_.thresholded, frame_.segmented, 15);
	if (res)
	{
		frame_.gesture = detectGesture(
			frame_.thresholded,
			frame_.segmented,
			frame_.hulls,
			frame_.defect,
			frame_.centerPoint,
			frame_.radius);
	}
}

void HandDetection::drawResult()
{
	cv::putText(
		frame_.origin,
		cv::String(gestureToStr(frame_.gesture)),
		originROI - cv::Point(100, -100),
		cv::FONT_HERSHEY_SCRIPT_SIMPLEX,
		5,
		CV_RGB(0, 0, 255),
		5);
	cv::drawContours(frame_.origin, frame_.segmented, 0, CV_RGB(0, 0, 255), 1, cv::LINE_8, cv::noArray(), INT_MAX, originROI);
	cv::drawContours(frame_.origin, frame_.hulls, 0, CV_RGB(0, 0, 255), 1, cv::LINE_8, cv::noArray(), INT_MAX, originROI);
	auto newCenterPoint = frame_.centerPoint + originROI;
	cv::circle(frame_.origin, newCenterPoint, int(frame_.radius), cv::Scalar(242, 216, 165), 2);
	cv::circle(frame_.origin, newCenterPoint, 2, cv::Scalar(0, 150, 0), 4, cv::FILLED);

	//if (!frame_.hulls.empty())
	//{
	//	for (const auto& i : frame_.hulls[0])
	//	{
	//		auto center = i + originROI;
	//		cv::circle(frame_.origin, center, 2, CV_RGB(255, 0, 0), 4, cv::FILLED);
	//	}
	//}

	for (const auto& i : frame_.defect)
	{
		auto startPoint = frame_.segmented[0][i[0]] + originROI;
		auto endPoint = frame_.segmented[0][i[1]] + originROI;
		auto farPoint = frame_.segmented[0][i[2]] + originROI;
		double depth = i[3] / 256;
		auto FS = euclideanDist(farPoint, startPoint);
		auto SE = euclideanDist(startPoint, endPoint);
		auto FE = euclideanDist(farPoint, endPoint);
		auto angleFar = ::acos((FS * FS + FE * FE - SE * SE) / (2 * FS * FE)) * 180.0 / CV_PI;	// Goc FarPoint
		if (angleFar <= 90/* && depth > 30*/)
		{
			cv::line(frame_.origin, startPoint, farPoint, CV_RGB(0, 255, 0), 2);
			cv::line(frame_.origin, endPoint, farPoint, CV_RGB(0, 255, 0), 2);
			cv::circle(frame_.origin, startPoint, 4, CV_RGB(0, 255, 0), 4);
			cv::circle(frame_.origin, endPoint, 4, CV_RGB(0, 255, 0), 4);
			cv::circle(frame_.origin, farPoint, 4, CV_RGB(100, 100, 100), 4);
		}
	}

	auto focus = frame_.origin(detectArea);
	cv::resize(focus, focus, cv::Size(450, 900));
	cv::imshow("Video Feed", frame_.origin);
	//cv::imshow("handArea Feed", frame_.handArea);
	//if (!frame_.thresholded.empty())
	//{
	//	cv::imshow("thresholded Feed", frame_.thresholded);
	//}
}
