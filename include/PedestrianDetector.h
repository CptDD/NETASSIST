#ifndef H_PEDESTRIAN_DETECTOR
#define H_PEDESTRIAN_DETECTOR

#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>

using namespace std;
using namespace cv;


class PedestrianDetector
{
protected:
	cv::HOGDescriptor hog;
	cv::Size window_stride;
	cv::Size padding;
	double scale;
	vector<Rect> detected;
	vector<Rect> non_max_suppress();
	Mat roi;
	Rect roi_rect;

public:
	PedestrianDetector(cv::Size window_stride=cv::Size(8,8),cv::Size padding=cv::Size(8,8),double scale=1.2)
	{
		hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
		this->window_stride=window_stride;
		this->padding=padding;
		this->scale=scale;
	}

	Mat get_pedestrian_roi(){return roi;}
	Rect get_pedestrian_roi_rect(){return roi_rect;}
	bool detect_pedestrian(Mat frame);
};

#endif