#include "FeatureTracker.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;



void FeatureTracker::draw_features(Mat &src)
{
	for(int i=0;i<next_features.size();i++)
	{
		//circle(src,initial_features[i],3,Scalar(255,0,0),-1);
		//line(src,initial_features[i],current_features[i],Scalar(0,0,255));
		circle(src,next_features[i],3,Scalar(0,0,255),-1);
	}
}

void FeatureTracker::track(Mat &current,Mat &previous)
{
	Mat current_gray,previous_gray;

	to_gray(current,current_gray);
	to_gray(previous,previous_gray);

	calcOpticalFlowPyrLK(previous_gray,current_gray,current_features,next_features,
		feature_status,feature_errors,winSize,LEVEL,criteria,0);

	this->test_features();

	this->draw_features(current);

	swap(next_features,current_features);

	if(current_features.size()<=10)
	{	
		cout<<"Resampling!"<<endl;
		this->extract_corners();
		this->transform_corners();
		this->initial_features.insert(initial_features.end(),corners.begin(),corners.end());
		this->current_features.insert(current_features.end(),corners.begin(),corners.end());
	}

}

void FeatureTracker::to_gray(Mat roi)
{
	cvtColor(roi,roi_gray,cv::COLOR_RGB2GRAY);
}

void FeatureTracker::to_gray(Mat &src,Mat &dst)
{
	cvtColor(src,dst,cv::COLOR_RGB2GRAY);
}


void FeatureTracker::set_roi(Mat roi,Rect roi_rect)
{
	this->roi=roi;
	this->roi_rect=roi_rect;
	this->to_gray(roi);
	this->extract_corners();
	this->transform_corners();
	this->initial_features=this->corners;
	this->current_features=initial_features;
}	


void FeatureTracker::extract_corners()
{
	goodFeaturesToTrack(roi_gray,corners,MAX_CORNERS,QUALITY,MIN_DISTANCE);
	cornerSubPix(roi_gray, corners, winSize, zeroZone, criteria );
}


void FeatureTracker::transform_corners()
{
	vector<Point2f> result;

	for(int i=0;i<corners.size();i++)
	{	
		Point2f temp;
		temp.x=roi_rect.tl().x+corners[i].x;
		temp.y=roi_rect.tl().y+corners[i].y;
		result.push_back(temp);
	}

	corners=result;
}


bool FeatureTracker::displacement_validity(Point2f current,Point2f next)
{
	if(abs(current.x-next.x)>0 &&abs(current.y-next.y)>0)
	{
		return true;
	}else
	{
		return false;
	}
}

void FeatureTracker::test_features()
{
	int good_count=0;

	cout<<"Next Features Before test :"<<next_features.size()<<endl;
	for(int i=0;i<next_features.size();i++)
	{	
		if(feature_status[i] && displacement_validity(current_features[i],next_features[i]))
		{
			initial_features[good_count]=initial_features[i];
			next_features[good_count++]=next_features[i];
		}
	}

	cout<<"Good count :"<<good_count<<endl;

	next_features.resize(good_count);
	initial_features.resize(good_count);

	cout<<"Next features after test :"<<next_features.size()<<endl;
}



