#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv/cv.h>

#include "PedestrianDetector.h"
#include "FeatureTracker.h"


using namespace std;
using namespace cv;




int main(int argc,char**argv)
{
	VideoCapture cap(argv[1]);

	Mat current,current_gray,previous,previous_gray,roi,roi_gray;
	PedestrianDetector pedestrianDetector;
	FeatureTracker tracker;


	bool detection=false;
	bool track=false;
	int n=0;

	if(cap.isOpened())
	{
		cap>>current;
		cv::resize(current,current,Size(640,480));
		previous=current;
	}


	while(cap.isOpened()&&char(waitKey(33))!='q')
	{
		cap>>current;
		cv::resize(current,current,Size(640,480));


		if(!detection)
		{
			detection=pedestrianDetector.detect_pedestrian(current);
		}else
		{
			if(!track)
			{
				tracker.set_roi(pedestrianDetector.get_pedestrian_roi(),pedestrianDetector.get_pedestrian_roi_rect());
				track=true;
			}else
			{
				tracker.track(current,previous);
			}
		}

		imshow("Current",current);
		
		previous=current;


	}
}