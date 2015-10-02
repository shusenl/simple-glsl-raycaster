#ifndef SLICE2D_H
#define SLICE2D_H

template<typename T>
class slice2D
{
public:
	slice2D(T*data, int dimY, int dimX);
	slice2D(const char* , int dimY, int dimX);
	slice2D(int dimY, int dimX);
	~slice2D();

	slice2D* generateShrinkedSlice(int minX, int maxX, int minY, int maxY);

	int getDimX();
	int getDimY();

	T getPixel(int y, int x);
	void setPixel(int y, int x, T value);

	T getPixel(double y, double x);
	T getPixelNN(double y, double x);
	T getPixelBilinear(double y, double x);
	slice2D<T>* resizeBilinear(double factor);
	slice2D<T>* resizeNN(double factor);

	//debug
	bool writeToFile(const char* filename);
	void print();

private:
	T* _data;
	int _pixelByteSize;
	int _dimX, _dimY;

	
};

#include <stdlib.h>
#include <assert.h>
#include <iostream>
using namespace std;

template<typename T>
slice2D<T>::slice2D(T*data, int dimY, int dimX)
: _data(data), _dimY(dimY), _dimX(dimX)
{
	_pixelByteSize = sizeof(T);
}

template<typename T>
slice2D<T>::slice2D(const char* filename , int dimY, int dimX)
{
	_pixelByteSize = sizeof(T);
	FILE *pFile; 

	pFile= fopen(filename, "rb");
	if (NULL == pFile) {
		cout<<"can't open file!"<<endl;
		//return false;
	}

	_data = new T[dimY*dimX];

	int size = fread(_data, _pixelByteSize, _dimX*_dimY, pFile);
	if (size !=  _dimX*_dimY)
	{
		printf("Read error!\n");
	}

}


template<typename T>
slice2D<T>::slice2D(int dimY, int dimX)
:_dimY(dimY), _dimX(dimX)
{
	_pixelByteSize = sizeof(T);
	_data = new T[dimY*dimX];
	memset(_data, 0, _pixelByteSize*dimY*dimX);
}

template<typename T>
slice2D<T>::~slice2D()
{
	if(_data)
		free(_data);
}

template<typename T>
slice2D<T>* slice2D<T>::generateShrinkedSlice(int minX, int maxX, int minY, int maxY)
{
	slice2D<T>* slice = new slice2D<T>(maxX-minX+1, maxY-minY+1);
	for(int i=minX; i<=maxX; i++)
		for(int j=minY; j<=maxY; j++)
			slice->setPixel(j-minY,i-minX, getPixel(j,i));
	return slice;
}


template<typename T>
int slice2D<T>::getDimX()
{
	return _dimX;
}

template<typename T>
int slice2D<T>::getDimY()
{
	return _dimY;
}

template<typename T>
T slice2D<T>::getPixel(int y, int x)
{
	if(x>=_dimX || y>=_dimY) //clamp to zero
		return 0;
	T value = 0;
	value = ((T*)(_data))[y*_dimX + x];
	return value;
}

template<typename T>
void slice2D<T>::setPixel(int y, int x, T value)
{
	assert(y<_dimY);
	assert(x<_dimX);
	((T*)(_data))[y*_dimX + x] = value;
}

//TODO
template<typename T>
T slice2D<T>::getPixel(double y, double x)
{
	T value = 0;

	return value;
}

//TODO
template<typename T>
T slice2D<T>::getPixelNN(double y, double x)
{
	T value = 0;
	return value;
}

template<typename T>
T slice2D<T>::getPixelBilinear(double y, double x)
{
	T value = 0;

	int j = (int)(y*(_dimY-1));
	int i = (int)(x*(_dimX-1));

	double fracX = x*(_dimX-1) - i;
	double fracY = y*(_dimY-1) - j;

	if ((fracX) == 0.0 && (fracY) == 0.0)
		return getPixel(j,    i);

	if( fracX == 0.0)
		return value = (T)(((double)getPixel(j,i)+(double)getPixel(j+1,i))*0.5);

	if( fracY == 0.0)
		return value = (T)(((double)getPixel(j,i)+(double)getPixel(j,i+1))*0.5);

/*	
	TL-TR
	 |      |
	BL-BR
*/
	double TL = (double)getPixel(j,    i);
	double TR = (double)getPixel(j,   i+1);
	double BL = (double)getPixel(j+1,i);
	double BR = (double)getPixel(j+1,i+1);

	value = (T)((TL*(1-fracX) + TR*fracX)*(1-fracY) + (BL*(1-fracX) + BR*fracX)*fracY);

	return value;
}

template<typename T>
slice2D<T>* slice2D<T>::resizeBilinear(double factor)
{
	int dimY = (int)(_dimY*factor);
	int dimX = (int)(_dimX*factor);

	slice2D<T>* slice = new slice2D<T>(1,dimY, dimX);
	for(int i=0; i<dimX; i++)
		for(int j=0; j<dimY; j++)
			slice->setPixel(j,i,this->getPixelBilinear(double(j)/double(_dimY),double(i)/double(_dimX)));
			//slice->setPixel(j,i,getPixel(j,i));
	return slice;
}

template<typename T>
slice2D<T>* slice2D<T>::resizeNN(double factor)
{
	int dimY = (int)(_dimY*factor);
	int dimX = (int)(_dimX*factor);

	slice2D<T>* slice = new slice2D<T>(1,dimY, dimX);
	for(int i=0; i<dimX; i++)
		for(int j=0; j<dimY; j++)
			//slice->setPixel(j,i,this->getPixelBilinear(double(j)/double(_dimY),double(i)/double(_dimX)));
			slice->setPixel(j,i,getPixel(j,i));
	return slice;
}


template<typename T>
bool slice2D<T>::writeToFile(const char* filename)
{
	FILE *pFile; 
	//fopen_s(&pFile, filename,"rb");
	pFile= fopen(filename, "wb");
	if (NULL == pFile) {
		cout<<"can't open file!"<<endl;
		return false;
	}

	int size = fwrite(_data, _pixelByteSize, _dimX*_dimY, pFile);
	if (size !=  _dimX*_dimY)
		return false;
	return true;
}

template<typename T>
void slice2D<T>::print()
{
	cout<<"Debug output: <<<<<<<<<"<<endl;
	for(int j=0; j<_dimY; j++)
	{
		for(int i=0; i<_dimX; i++)
		{
			cout<<this->getPixel(j,i)<<" ";
		}
		cout<<endl;
	}
}



#endif
