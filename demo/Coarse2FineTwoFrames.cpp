#include "project.h"
#include "Image.h"
#include "OpticalFlow.h"
#include <iostream>

void main()
{
	DImage Im1;
	Im1.loadImage("demo/car1.jpg");
	DImage Im2;
	Im2.loadImage("demo/car2.jpg");
	double alpha= 1;
	double ratio=0.5;
	int minWidth= 40;
	int nOuterFPIterations = 3;
	int nInnerFPIterations = 1;
	int nSORIterations= 20;
	
	DImage vx,vy,warpI2;
	OpticalFlow::Coarse2FineFlow(vx,vy,warpI2,Im1,Im2,alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations);

	// output the parameters

}