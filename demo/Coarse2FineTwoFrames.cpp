#include "Image.h"
#include "OpticalFlow.h"
#include <iostream>
#include <opencv2/highgui.hpp>

cv::Mat toCvMat(const DImage& im) {
	return cv::Mat(im.height(), im.width(), CV_64FC3, im.pData);
}

void main()
{
	DImage Im1;
	Im1.imread("demo/car1.jpg");
	DImage Im2;
	Im2.imread("demo/car2.jpg");
	double alpha= 1;
	double ratio=0.5;
	int minWidth= 40;
	int nOuterFPIterations = 3;
	int nInnerFPIterations = 1;
	int nSORIterations= 20;
	
	DImage vx,vy,warpI2;
	OpticalFlow::Coarse2FineFlow(vx,vy,warpI2,Im1,Im2,alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations);
	warpI2.saveImage("demo/test");
	cv::imshow("old image", toCvMat(Im2));
	cv::imshow("new image", toCvMat(warpI2));
	cv::waitKey(0);
	// output the parameters

}