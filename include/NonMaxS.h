#ifndef H_NONMAXSUP
#define H_NONMAXSUP

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class NonMaxSuppressor
{
protected:
	double compute_area(Rect rect){return (rect.br().x-rect.tl().x)*(rect.br().y-rect.tl().y);}
	void info_single_rect(Rect rect){std::cout<<rect.tl()<<" "<<rect.br()<<endl;}
	void info_detection(vector<Rect> detection);
	bool valid(Rect r1,Rect r2);
public:
	vector<Rect> suppress(vector<Rect> detected);
};


#endif