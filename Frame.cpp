#include "Frame.h"

int Frame::count = 0;
cv::Mat Frame::bkg = cv::Mat();

const cv::Size blurSize(35, 35);

Frame::Frame(cv::Mat& f)
	: origin(f)
    , gesture(GESTURE::ZERO)
{
	count++;
	cv::flip(origin, origin, 1);
	cv::rectangle(origin, detectArea, CV_RGB(0, 255, 0), 2);
	handArea = origin(detectArea);
	cv::cvtColor(handArea, handArea, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(handArea, handArea, blurSize, 0);
}
