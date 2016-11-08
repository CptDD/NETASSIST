#ifndef H_LKTRACKER
#define H_LKTRACKER

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>


#define MAX_CORNERS 15
#define MIN_DISTANCE 5.0
#define QUALITY 0.05
#define LEVEL 2


using namespace std;
using namespace cv;

class FeatureTracker
{

protected:
	Mat roi,roi_gray;
	vector<Point2f> corners;
	vector<Point2f> initial_features;
	vector<Point2f> current_features,next_features;

	vector<uchar> feature_status;
	vector<float> feature_errors;
	Size winSize;
  	Size zeroZone;
  	TermCriteria criteria;

  	Rect roi_rect;

  	void extract_corners();
  	void to_gray(Mat roi);
  	void to_gray(Mat &src,Mat &dst);
  	void transform_corners();

  	void test_features();
  	void draw_features(Mat &src);

  	bool displacement_validity(Point2f current,Point2f next);

public:

	FeatureTracker(cv::Size winSize=cv::Size(10,10),
		           cv::Size zeroZone=cv::Size(-1,-1),
		           TermCriteria criteria=TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,40,0.001))
	{
		this->winSize=winSize;
		this->zeroZone=zeroZone;
		this->criteria=criteria;
	}


	void track(Mat &current,Mat &previous);
	void set_roi(Mat roi,Rect roi_rect);
	vector<Point2f> get_corners(){return corners;}
};


#endif