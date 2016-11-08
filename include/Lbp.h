#ifndef H_LBP
#define H_LBP

#include <iostream>
#include <opencv2/opencv.hpp>

class Lbp
{
protected:
	int pattern_size;
	cv::Mat to_gray(cv::Mat src);
	
public:
	Lbp(){this->pattern_size=3;}
	cv::Mat compute_olbp(cv::Mat src);
	cv::Mat compute_elbp(cv::Mat src,int radius,int neighbours);

	void compute_histogram(cv::Mat src);

	int get_pattern_size(){return pattern_size;}
	void set_pattern_size(int pattern_size){this->pattern_size=pattern_size;}


};

#endif