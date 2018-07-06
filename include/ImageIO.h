#ifndef _ImageIO_h
#define _ImageIO_h

#include <cv.hpp>
#include <highgui.h>
#include <type_traits>

class ImageIO
{
public:
	enum ImageType{standard, derivative, normalized};
	ImageIO(void);
	~ImageIO(void);
public:
	template <class T>
	static bool loadImage(const char* filename,T*& pImagePlane,int& width,int& height, int& nchannels);
	template <class T>
	static bool saveImage(const char* filename,const T* pImagePlane,int width,int height, int nchannels,ImageType imtype = standard);

};

template <class T>
bool ImageIO::loadImage(const char *filename, T *&pImagePlane, int &width, int &height, int &nchannels)
{
	cv::Mat im = cv::imread(filename);
	if(im.data == nullptr) // if allocation fails
		return false;
	if(im.type()!= CV_8UC1 && im.type()!=CV_8UC3 && im.type()!=CV_8UC4) // we only support three types of image information for now
		return false;
	width = im.size().width;
	height = im.size().height;
	nchannels = im.channels();
	pImagePlane = new T[width*height*nchannels];

	if constexpr(std::is_same_v<T, unsigned char>)
	{
		for(int i = 0;i<height;i++)
			memcpy(pImagePlane+i*im.step,im.data+i*im.step,width*nchannels);
		return true;
	}

	// check whether the type is float point
	constexpr bool IsFloat = std::is_floating_point_v<T>;
	
	for(int i =0;i<height;i++)
	{
		int offset1 = i*width*nchannels;
		int offset2 = i*im.step;
		for(int j=0;j<im.step;j++)
		{
			if constexpr(IsFloat)
				pImagePlane[offset1+j] = (T)im.data[offset2+j]/255;
			else
				pImagePlane[offset1+j] = im.data[offset2+j];
		}
	}
	return true;
}

template <class T>
bool ImageIO::saveImage(const char* filename,const T* pImagePlane,int width,int height, int nchannels,ImageType imtype)
{
	cv::Mat im;
	switch(nchannels){
		case 1:
			im.create(height,width,CV_8UC1);
			break;
		case 3:
			im.create(height,width,CV_8UC3);
			break;
		default:
			return false;
	}
	// check whether the type is float point
	constexpr bool IsFloat = std::is_floating_point_v<T>;

	T Max,Min;
	int nElements = width*height*nchannels;
	switch(imtype){
		case standard:
			break;
		case derivative:
			// find the max of the absolute value
			Max = pImagePlane[0];
			if constexpr(!IsFloat)
				for(int i = 0;i<nElements;i++)
					Max = std::max<T>(Max,std::abs(pImagePlane[i]));
			else
				for(int i=0;i<nElements;i++)
					Max = std::max<T>(Max,std::abs(pImagePlane[i]));
			Max*=2;
			break;
		case normalized:
			Max = Min = pImagePlane[0];
			for(int i = 0;i<nElements;i++)
			{
				Max = std::max(Max,pImagePlane[i]);
				Min = std::min(Min,pImagePlane[i]);
			}
			break;
	}
	if(std::is_same_v<T, unsigned char> && imtype == standard)
	{
		for(int i = 0;i<height;i++)
			memcpy(im.data+i*im.step,pImagePlane+i*im.step,width*nchannels);
	}
	else
	{
		for(int i =0;i<height;i++)
		{
			int offset1 = i*width*nchannels;
			int offset2 = i*im.step;
			for(int j=0;j<im.step;j++)
			{
				switch(imtype){
					case standard:
						if constexpr(IsFloat)
							im.data[offset2+j] = pImagePlane[offset1+j]*255;
						else
							im.data[offset2+j] = std::max<T>(std::min<T>(pImagePlane[offset1+j],(T)255),(T)0);
						break;
					case derivative:
						
						if constexpr(IsFloat)
							im.data[offset2+j] = (double)(pImagePlane[offset1+j]/Max+0.5)*255;
						else
							im.data[offset2+j] = ((double)pImagePlane[offset1+j]/Max+0.5)*255;
						break;
					case normalized:
						im.data[offset2+j] = (double)(pImagePlane[offset1+j]-Min)/(Max-Min)*255;
						break;
				}
			}
		}
	}
	return cv::imwrite(filename,im);
}



/*
#include <QVector>
#include <QImage>
#include <QString>
#include "math.h"
//-----------------------------------------------------------------------------------------
// this class is a wrapper to use QImage to load image into image planes
//-----------------------------------------------------------------------------------------

class ImageIO
{
public:
	enum ImageType{standard, derivative, normalized};
	ImageIO(void);
	~ImageIO(void);
public:
	template <class T>
	static void loadImage(const QImage& image,T*& pImagePlane,int& width,int& height,int& nchannels);
	template <class T>
	static bool loadImage(const QString& filename,T*& pImagePlane,int& width,int& height,int& nchannels);

	template <class T>
	static unsigned char convertPixel(const T& value,bool IsFloat,ImageType type,T& _Max,T& _Min);

	template <class T>
	static bool writeImage(const QString& filename, const T*& pImagePlane,int width,int height,int nchannels,ImageType type=standard,int quality=-1);

	template <class T>
	static bool writeImage(const QString& filename,const T* pImagePlane,int width,int height,int nchannels,T min, T max,int quality=-1);

};

template <class T>
void ImageIO::loadImage(const QImage& image, T*& pImagePlane,int& width,int& height,int& nchannels)
{
	// get the image information
	width=image.width();
	height=image.height();
	nchannels=3;
	pImagePlane=new T[width*height*nchannels];

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	const unsigned char* plinebuffer;
	for(int i=0;i<height;i++)
	{
		plinebuffer=image.scanLine(i);
		for(int j=0;j<width;j++)
		{
			if(IsFloat)
			{
				pImagePlane[(i*width+j)*3]=(T)plinebuffer[j*4]/255;
				pImagePlane[(i*width+j)*3+1]=(T)plinebuffer[j*4+1]/255;
				pImagePlane[(i*width+j)*3+2]=(T)plinebuffer[j*4+2]/255;
			}
			else
			{
				pImagePlane[(i*width+j)*3]=plinebuffer[j*4];
				pImagePlane[(i*width+j)*3+1]=plinebuffer[j*4+1];
				pImagePlane[(i*width+j)*3+2]=plinebuffer[j*4+2];
			}
		}
	}
}

template <class T>
bool ImageIO::loadImage(const QString&filename, T*& pImagePlane,int& width,int& height,int& nchannels)
{
	QImage image;
	if(image.load(filename)==false)
		return false;
	if(image.format()!=QImage::Format_RGB32)
	{
		QImage temp=image.convertToFormat(QImage::Format_RGB32);
		image=temp;
	}
	loadImage(image,pImagePlane,width,height,nchannels);
	return true;
}

template <class T>
bool ImageIO::writeImage(const QString& filename, const T*& pImagePlane,int width,int height,int nchannels,ImageType type,int quality)
{
	int nPixels=width*height,nElements;
	nElements=nPixels*nchannels;
	unsigned char* pTempBuffer;
	pTempBuffer=new unsigned char[nPixels*4];
	memset(pTempBuffer,0,nPixels*4);

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	T _Max=0,_Min=0;
	switch(type){
		case standard:
			break;
		case derivative:
			_Max=0;
			for(int i=0;i<nPixels;i++)
			{
				if(IsFloat)
					_Max=max(_Max,fabs((double)pImagePlane[i]));
				else
					_Max=max(_Max,abs(pImagePlane[i]));
			}
			break;
		case normalized:
			_Min=_Max=pImagePlane[0];
			for(int i=1;i<nElements;i++)
			{
				_Min=min(_Min,pImagePlane[i]);
				_Max=max(_Max,pImagePlane[i]);
			}
			break;
	}

	for(int i=0;i<nPixels;i++)
	{
		if(nchannels>=3)
		{
			pTempBuffer[i*4]=convertPixel(pImagePlane[i*nchannels],IsFloat,type,_Max,_Min);
			pTempBuffer[i*4+1]=convertPixel(pImagePlane[i*nchannels+1],IsFloat,type,_Max,_Min);
			pTempBuffer[i*4+2]=convertPixel(pImagePlane[i*nchannels+2],IsFloat,type,_Max,_Min);
		}
		else 
			for (int j=0;j<3;j++)
				pTempBuffer[i*4+j]=convertPixel(pImagePlane[i*nchannels],IsFloat,type,_Max,_Min);
		pTempBuffer[i*4+3]=255;
	}
	QImage *pQImage=new QImage(pTempBuffer,width,height,QImage::Format_RGB32);
	bool result= pQImage->save(filename,0,quality);
	delete pQImage;
	delete pTempBuffer;
	return result;
}

template <class T>
bool ImageIO::writeImage(const QString& filename, const T* pImagePlane,int width,int height,int nchannels,T min,T max,int quality)
{
	int nPixels=width*height,nElements;
	nElements=nPixels*nchannels;
	unsigned char* pTempBuffer;
	pTempBuffer=new unsigned char[nPixels*4];
	memset(pTempBuffer,0,nPixels*4);

	// check whether the type is float point
	bool IsFloat=false;
	if(typeid(T)==typeid(double) || typeid(T)==typeid(float) || typeid(T)==typeid(long double))
		IsFloat=true;

	T _Max=max,_Min=min;

	for(int i=0;i<nPixels;i++)
	{
		if(nchannels>=3)
		{
			pTempBuffer[i*4]=convertPixel(pImagePlane[i*nchannels],IsFloat,normalized,_Max,_Min);
			pTempBuffer[i*4+1]=convertPixel(pImagePlane[i*nchannels+1],IsFloat,normalized,_Max,_Min);
			pTempBuffer[i*4+2]=convertPixel(pImagePlane[i*nchannels+2],IsFloat,normalized,_Max,_Min);
		}
		else 
			for (int j=0;j<3;j++)
				pTempBuffer[i*4+j]=convertPixel(pImagePlane[i*nchannels],IsFloat,normalized,_Max,_Min);
		pTempBuffer[i*4+3]=255;
	}
	QImage *pQImage=new QImage(pTempBuffer,width,height,QImage::Format_RGB32);
	bool result= pQImage->save(filename,0,quality);
	delete pQImage;
	delete pTempBuffer;
	return result;
}

template <class T>
unsigned char ImageIO::convertPixel(const T& value,bool IsFloat,ImageType type,T& _Max,T& _Min)
{
	switch(type){
		case standard:
			if(IsFloat)
				return max(min(value*255,255),0);
			else
				return max(min(value,255),0);
			break;
		case derivative:
			return (double)((double)value/_Max+1)/2*255;
			break;
		case normalized:
			return (double)(value-_Min)/(_Max-_Min)*255;
			break;
	}
	return 0;
}
//*/
#endif