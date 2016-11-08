#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv/cv.h>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/core.hpp>



#include "ParticleFilter.h"
#include "NonMaxS.h"
#include "Lbp.h"

#define QUALITY 0.01

using namespace std;
using namespace cv;


cv::Point position1, position2;

// -------------Kalman estimation creation--------------------
KalmanFilter KF(4,2,0,CV_32F);//=CreateKalman(4, 2, 0);
Mat_<float> state(4, 1); /* (x, y, Vx, Vy) */
Mat processNoise(4, 1, CV_32F);
Mat_<float> measurmentVP(2,1);
vector<Point> kalmanVec;
int nrNotDetect=0;
Point observation, estimation;
double lastTimeStamp=0.0;

vector<Rect> detected,pick;

int x_offset,y_offset;


void initKF()
{
	measurmentVP.setTo(Scalar(0.0));
	KF.statePre.at<float>(0) = 16;
	KF.statePre.at<float>(1) = 11;
	KF.statePre.at<float>(2) = 0;
	KF.statePre.at<float>(3) = 0;
	// 4x4 for the constant velocity model
	KF.transitionMatrix = (Mat_<float>(4, 4) << 1,0,1,0,   0,1,0,1,  0,0,1,0,  0,0,0,1);

	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-1));
	// proces noise setup, faster response
	// can be finetuned these values for the actual config
	KF.processNoiseCov.at<float>(10) = pow( 10, -2);
	KF.processNoiseCov.at<float>(15) = pow( 10, -2);
	KF.processNoiseCov.at<float>(0) = pow( 10, -2.5);
	KF.processNoiseCov.at<float>(5) = pow( 10, -2.5);
	// measurment noise setup - look for the actual values from the experiments
	setIdentity(KF.measurementNoiseCov, Scalar::all(2e-2)); 	
	setIdentity(KF.errorCovPost, Scalar::all(.1));
	kalmanVec.clear();
}


Point KFestimation(Point at){
	//estimation
	Mat prediction = KF.predict();
	Point predictPt(prediction.at<float>(0),prediction.at<float>(1));
	measurmentVP(0) = at.x;
	measurmentVP(1) = at.y;
	// measurement += KF.measurementMatrix*state;
	Mat estimated = KF.correct(measurmentVP);
	Point statePt(estimated.at<float>(0),estimated.at<float>(1));     
	kalmanVec.push_back(statePt);
	return statePt ;
}


void imageCallback(Mat frame)
{

  Scalar red=CV_RGB(255,0,0);
  cv::rectangle(frame,estimation, Point(estimation.x+x_offset,estimation.y+y_offset),Scalar(220,0,0),3);
//  cv::rectangle(frame,position1, position2,red,3);
  cv::circle(frame,position1,3,Scalar(220,0,0),-1,8,0);
  cv::circle(frame,position2,3,Scalar(120,0,0),-1,8,0);

  cv::imshow("Image window", frame);
  cv::waitKey(3);
}

void detectionCallback(bool detection,Rect rect)
{
	// scale it down and take the midle part
			
		position1=rect.tl();
		position2.x=rect.tl().x+rect.width/2;
		position2.y=rect.tl().y+rect.height/2;
		
		estimation = KFestimation(Point(position1.x,position1.y));
}

void detectionCallback()
{
	position1.x=-1;
	position1.y=-1;
	position2.x=-1;
	position2.y=-1;

	Mat prediction=KF.predict();
	estimation=Point(prediction.at<float>(0),prediction.at<float>(1));
}

void draw_trackers(Mat &src,vector<Point2f> corners)
{
		for(int i=0;i<corners.size();i++)
		{
			circle(src,corners[i],3,Scalar(0,0,255),-1);
		}
}

void draw_features(Mat &src,vector<Point2f> corners,vector<Point2f> initial_set)
{
	for(int i=0;i<corners.size();i++)
	{
		circle(src,corners[i],3,Scalar(0,0,255),-1);
		line(src,initial_set[i],corners[i],Scalar(0,0,255));
		circle(src,initial_set[i],3,Scalar(255,0,0),-1);
	}
}

vector<Point2f> transform_points(vector<Point2f> points,Rect rect)
{

	vector<Point2f> result;
	for(int i=0;i<points.size();i++)
	{	
		Point2f temp;
		temp.x=rect.tl().x+points[i].x;
		temp.y=rect.tl().y+points[i].y;
		result.push_back(temp);
	}

	return result;
}


vector<Point2f> resample_points(Mat src)
{
	vector<Point2f> corners;
	int max_corners=30;
	double quality=0.01;
	int min_distance=5;

	goodFeaturesToTrack(src,corners,max_corners,quality,min_distance);

	return corners;

}



/*int main(int arcg,char**argv)
{
	cout<<"===Pedestrian Detection demo===\n = = = HOG Features = = = "<<endl;	

	//Mat frame=imread(argv[1],IMREAD_COLOR);

	Mat roi,roi_gray,frame,frame_gray,previous,preious_gray;
	Lbp lbp_computation;

	VideoCapture cap(argv[1]);


	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	namedWindow("Detected person",WINDOW_AUTOSIZE);

	NonMaxSuppressor sup;
	

	//initKF();

	bool det=false;

	vector<Point2f> corners,transformed_corners,initial_set;
	vector<Point2f> points[2];
	int max_corners=20;
	double quality=0.01;
	int min_distance=5;
	vector<uchar> status;
	vector<float> err;
	Size winSize = Size( 10, 10 );
  	Size zeroZone = Size( -1, -1 );
  	TermCriteria criteria = TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001 );

	if(!cap.isOpened())
	{
		cout<<"Am error has occured while opening the camera!"<<endl;
	}else
	{
		cap>>frame;
		cv::resize(frame,frame,Size(640,480));
	}

	while(cap.isOpened()&&char(waitKey(33))!='q')
	{
		previous=frame;
		cap>>frame;
		
		if(frame.empty())
		{
			break;
		}
		cv::resize(frame,frame,Size(640,480));




		if(!det)
		{
			hog.detectMultiScale(frame, detected, 0, cv::Size(8,8), cv::Size(8,8), 1.2, 2.0);

			vector<Rect>::const_iterator it;

			for(it=detected.begin();it!=detected.end();++it)
			{
				Rect rect=*it;
				//rectangle(frame,rect.tl(),rect.br(),cv::Scalar(255,0,0),2);
			}

			pick=sup.suppress(detected);

			for(it=pick.begin();it!=pick.end();++it)
			{
				Rect rect=*it;
				//rectangle(frame,rect.tl(),rect.br(),cv::Scalar(0,255,0),2);
			}

			if(pick.size()>0)
			{
				roi=frame(pick[0]);
				cvtColor(roi,roi_gray,COLOR_RGB2GRAY);

				goodFeaturesToTrack(roi_gray,corners,max_corners,quality,min_distance);

			
  				cornerSubPix(roi_gray, corners, winSize, zeroZone, criteria );
  				transformed_corners=transform_points(corners,pick[0]);

  				points[0]=transformed_corners;
  				initial_set.insert(initial_set.end(),transformed_corners.begin(),transformed_corners.end());

				/*for(int i=0;i<corners.size();i++)
				{
					cout<<corners[i].x<<" "<<corners[i].y<<endl;
					cout<<corners[i].x+pick[0].tl().x<<" "<<corners[i].y+pick[0].tl().y<<endl;

					circle(frame,corners[i],3,Scalar(0,0,255),-1);
					circle(frame,corners[i],3,Scalar(0,255,0),-1);

					imshow("Features",frame);
					waitKey();
				}

		
				//draw_features(frame,corners);
				//imshow("Region",roi);
				//imshow("Region Gray",roi_gray);

					draw_trackers(frame,transformed_corners);
					imshow("Example",frame);
					waitKey();

				det=true;
			}

		}else
		{
			cvtColor(previous,preious_gray,COLOR_RGB2GRAY);
			cvtColor(frame,frame_gray,COLOR_RGB2GRAY);

			calcOpticalFlowPyrLK(preious_gray,frame_gray,points[0],points[1],status,err,winSize,2,criteria,0);

			int nr=0;
			for(int i=0;i<status.size();i++)
			{
				bool abs_diff=((abs(points[0][i].x-points[1][i].x)+abs(points[0][i].y-points[1][i].y))>2);

				if(status[i]&&abs_diff)
				{
					initial_set[nr]=initial_set[i];
					points[1][nr++]=points[1][i];
				}
			}

			points[1].resize(nr);
			initial_set.resize(nr);
			draw_features(frame,points[1],initial_set);
			//draw_trackers(frame,points[1]);
			std::swap(points[1], points[0]);
			//transform_points(points[0],pick[0]);

			cout<<"Features size :"<<points[0].size()<<endl;

			if(points[0].size()<10)
			{
				
			}
		}

		imshow("Detected person",frame);
		waitKey();
	}



	return 0;
}


		/*cv::resize(frame,frame,cv::Size(640,480));
		while(!det)
		{
			hog.detectMultiScale(frame, detected, 0, cv::Size(8,8), cv::Size(8,8), 1.2, 2.0);
			vector<Rect>::const_iterator it;

			for(it=detected.begin();it!=detected.end();++it)
			{
				Rect rect=*it;
				rectangle(frame,rect.tl(),rect.br(),cv::Scalar(255,0,0),2);
			}

			pick=sup.suppress(detected);

			for(it=pick.begin();it!=pick.end();++it)
			{
				Rect rect=*it;
				rectangle(frame,rect.tl(),rect.br(),cv::Scalar(0,255,0),2);
			}

			if(pick.size()>0)
			{
				cout<<"Detection occured!"<<endl;
				det=true;
			}
		}


	}


return 0;
}*/
	 