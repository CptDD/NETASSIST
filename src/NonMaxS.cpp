#include <iostream>
#include <opencv2/opencv.hpp>


#include "NonMaxS.h"

using namespace std;
using namespace cv;

bool comp(Rect x,Rect y){return x.br().y<y.br().y;}

bool NonMaxSuppressor::valid(Rect r1,Rect r2)
{
	if((r1.tl().x-r2.tl().x>=0&&r1.br().x-r2.br().x<=0)&&(r1.tl().y-r2.tl().y>=0&&r1.br().y-r2.br().y<=0))
	{
		return true;
	}else
	{
		return false;
	}
}

void NonMaxSuppressor::info_detection(vector<Rect> detected)
{
	for(int i=0;i<detected.size();i++)
	{
		cout<<detected[i].tl()<<" "<<detected[i].br()<<endl; 
	}
}

vector<Rect> NonMaxSuppressor::suppress(vector<Rect> detected)
{
	vector<double> area;
	for(int i=0;i<detected.size();i++)
	{
		area.push_back(compute_area(detected[i]));
	}

	sort(detected.begin(),detected.end(),comp);

	vector<Rect>pick;


	for(int j=detected.size()-1;j>=0;j--)
	{
		Rect last=detected[j];

		for(int i=0;i<j;i++)
		{
			Rect temp=detected[i];
			if(this->valid(temp,last))
			{
				cout<<"Suppress!"<<endl;
				detected.erase(remove(detected.begin(),detected.end(),temp),detected.end());
			}
		}

		pick.push_back(last);
		detected.erase(remove(detected.begin(),detected.end(),last),detected.end());
	}
	
	return pick;
}
