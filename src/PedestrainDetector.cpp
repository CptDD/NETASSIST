#include "PedestrianDetector.h"
#include "NonMaxS.h"

#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>

using namespace std;


bool PedestrianDetector::detect_pedestrian(Mat frame)
{
	hog.detectMultiScale(frame,detected,0,window_stride,padding,scale);

	detected=this->non_max_suppress();

	if(detected.size()>0)
	{
		roi=frame(detected[0]);
		roi_rect=detected[0];
		
		return true;
	}else
	{
		return false;
	}
}


vector<Rect> PedestrianDetector::non_max_suppress()
{

	NonMaxSuppressor sup;
	vector<Rect> suppressed;

	return sup.suppress(detected);

}