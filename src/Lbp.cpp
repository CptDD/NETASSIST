#include <iostream>
#include <limits>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#include "Lbp.h"

cv::Mat  Lbp::compute_olbp(Mat src)
{
	src=this->to_gray(src);

	Mat dst=Mat::zeros(src.rows-2,src.cols-2,CV_8UC1);

	int what;

	for(int i=1;i<src.rows-1;i++)
	{
		for(int j=1;j<src.cols-1;j++)
		{
			what=0;
			unsigned char center=src.at<uchar>(i,j);

			what|=(center<=src.at<uchar>(i-1,j-1))<<7;
			what|=(center<=src.at<uchar>(i-1,j))<<6;
			what|=(center<=src.at<uchar>(i-1,j+1))<<5;
			what|=(center<=src.at<uchar>(i,j+1))<<4;
			what|=(center<=src.at<uchar>(i+1,j+1))<<3;
			what|=(center<=src.at<uchar>(i+1,j))<<2;
			what|=(center<=src.at<uchar>(i+1,j-1))<<1;
			what|=(center<=src.at<uchar>(i,j-1))<<0;

			dst.at<uchar>(i-1,j-1)=what;
		}
	}

	return dst;
}


Mat Lbp::to_gray(Mat src)
{
	Mat res;
	cvtColor(src,res,COLOR_RGB2GRAY);
	return res;
}

void Lbp::compute_histogram(cv::Mat src)
{	

	/*int max=256;
	cout<<"Computing the histogram!"<<endl;

	Mat hist=Mat::zeros(Size(max,1),CV_32FC1);

	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			int val=src.at<uchar>(i,j);

			if(val<max)
			{
				hist.at<float>(val)+=1;
			}
		}
	}*/

	Mat hist;
	int channels[]={0};
	int histSize[]={256};
	float range[] = { 0, 256 };

	const float* ranges[] = { range };

    calcHist( &src, 1, channels, Mat(), 
         hist, 1, histSize, ranges,
         true, 
         false );

    imshow("A",hist);

}


cv::Mat Lbp::compute_elbp(Mat src,int radius, int neighbours) {

	Mat dst=Mat::zeros(src.rows-2*radius,src.cols-2*radius,CV_8UC1);

	for(int n=0;n<neighbours;n++)
	{
		float x=static_cast<float>(radius)*cos(2*M_PI*n/static_cast<float>(neighbours));
		float y=static_cast<float>(radius)*-sin(2*M_PI*n/static_cast<float>(neighbours));


		float fx=static_cast<int>(floor(x));
		float fy=static_cast<int>(floor(y));

		float cx=static_cast<int>(ceil(x));
		float cy=static_cast<int>(ceil(y));

		cout<<"FX :"<<fx<<" "<<x<<" CX :"<<cx<<endl;
		cout<<"FY :"<<fy<<" "<<y<<" CY :"<<cy<<endl;

		float w1=(cx-x)*(cy-y)/(cx-fx)*(cy-fy);
		float w2=(x-fx)*(y-fy)/(cx-fx)*(cy-fy);

		float ty = y - fy;
		float tx = x - fx;

		 w1 = (1 - tx) * (1 - ty);
		w2 = tx * (1 - ty);
		float w3 = (1 - tx) * ty;
		float w4 = tx * ty;

		cout<<w1<<" "<<w2<<" "<<w3<<" "<<w4<<endl;
	}	

	return dst;

}