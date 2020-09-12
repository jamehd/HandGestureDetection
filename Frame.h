#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

const cv::Rect detectArea(700, 100, 400, 400);
const cv::Point originROI(700, 100);

enum class GESTURE
{
	ZERO = 0,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	INVALID,
	NO_DATA,
	OKE = 100,
	BEST_OF_LUCK = 101,
};

inline std::string gestureToStr(const GESTURE& g)
{
	if (GESTURE::ZERO == g)
	{
		return std::string("0");
	}
	else if (GESTURE::ONE == g)
	{
		return std::string("1");
	}
	else if (GESTURE::TWO == g)
	{
		return std::string("2");
	}
	else if (GESTURE::THREE == g)
	{
		return std::string("3");
	}
	else if (GESTURE::FOUR == g)
	{
		return std::string("4");
	}
	else if (GESTURE::FIVE == g)
	{
		return std::string("5");
	}
	else if (GESTURE::OKE == g)
	{
		return std::string("Oke");
	}
	else if (GESTURE::INVALID == g)
	{
		return std::string("Invalid");
	}
	else if (GESTURE::NO_DATA == g)
	{
		return std::string("No Data to detect");
	}
	else if (GESTURE::BEST_OF_LUCK == g)
	{
		return std::string("Best Of Luck");
	}
	else
	{
		return std::string("");
	}
}

class Frame
{
public:
	Frame(cv::Mat&);


	cv::Mat& origin;
	cv::Mat handArea;
	cv::Mat thresholded;
	std::vector<std::vector<cv::Point>> segmented, hulls;
	cv::Point centerPoint;
	double radius;
	std::vector<cv::Vec4i> defect;
	GESTURE gesture;

	static cv::Mat bkg;
	static int count;
};

