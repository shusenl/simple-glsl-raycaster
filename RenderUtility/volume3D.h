#ifndef _VOLUME3D_H
#define _VOLUME3D_H
#include <stdio.h>
#include <cstring>
#include <math.h>
#include <string>
#include <iostream>
#include <assert.h>
#include <sstream>

#include <limits>

#include "slice2D.h"
//#pragma warning( disable : C4305 )
/**
Volume3D 
Author: Shusen Liu, SCI Institute, University of Utah
shusen.liu.hust@gmail.com
Feb.12. 2011
*/
using namespace std;


template <typename T>
class volume3D  /* :public volume3DInterface*/
{
public:
	volume3D(const char* filename, int z, int y, int x, bool delayLoad=false);
	volume3D(T *data, int z, int y, int x );
	volume3D(int z, int y, int x, int value ); //create empty volume
	~volume3D();

	void calculateVolumeStatistic();
  void normalize(T min ,T max);
  void normalize();
  volume3D<unsigned char>* generateUchar();
  volume3D<unsigned char>* generateUcharNormalized();
	/*
	*	volume generator / modifier
	*/
  void generateHistogram(int binNum=256);
	volume3D* generateVolume16bitTo8bit();
	volume3D* generateUpSampleVolume();
	volume3D* generateDownSampleVolume();
	volume3D* generateDownSampleVolumeNN();
	volume3D* generateDownSampleVolumeWithBoundaryNN();

	volume3D* generateSubVolume(double minZ, double maxZ,
													    double minY, double maxY,
													    double minX, double maxX);
	volume3D* generateSubVolume(int minZ, int maxZ,
													    int minY, int maxY,
													    int minX, int maxX);

	volume3D* generateOctBlock(int z, int y, int x, bool boundary);

	bool addSubVolume(volume3D<T>* vol, int z, int y, int x);
	void addSphereRegion(int cz, int cy, int cx, int radius,T value);

	bool clearSubRegion(int minZ, int maxZ, 
									int minY, int maxY,
									int minX, int maxX,
									T clearValue);


	void updateBoundaryToUpperLevelBlock(volume3D<T>* upperLevel, int zp, int zn, int yp, int yn, int xp, int xn); //x:0,1 y:0,1 z:0,1 
	//accessors 
	int getVoxelByteSize();
	T getVoxel(int z, int y, int x);
	T getVoxel(double z, double y, double x);
	T getVoxelNN(double z, double y, double x);
  int getDimX(){return _dimX;}
	int getDimY(){return _dimY;}
	int getDimZ(){return _dimZ;}

	void generateVirtualDim(int);
  int getVirtualDimX(){return _virtualDimX;}
	int getVirtualDimY(){return _virtualDimY;}
	int getVirtualDimZ(){return _virtualDimZ;}
  void* getDataBuffer(){return _data;}

	//modifier
	bool setVoxel(int z, int y, int x, T value);

	//status
	void 	updateMaxMin();
  T getMin(){return _min;}
  T getMax(){return _max;}
	void calculateMean();
  T getMean(){return _mean;}
  double getDynamicRange(){return _dynamicRange;}
	//slice generator/modifier
	slice2D<T>* generateSliceX(int index, int minY, int maxY, int minX, int maxX);
	slice2D<T>* generateSliceY(int index, int minY, int maxY, int minX, int maxX);
	slice2D<T>* generateSliceZ(int index, int minY, int maxY, int minX, int maxX);

	void updateSliceX(slice2D<T>* slice, int index, int minY, int maxY, int minX, int maxX);
	void updateSliceY(slice2D<T>* slice, int index, int minY, int maxY, int minX, int maxX);
	void updateSliceZ(slice2D<T>* slice, int index, int minY, int maxY, int minX, int maxX);

	//debug
	std::string number2String(int num);
	bool writeToFile(const char* filename, const char* path = "\0");


private:
	std::string _filename;
	
	int _voxelByteSize;
	T *_data;
  double *_histo;

	int _dimX, _dimY, _dimZ;
	int _virtualDimX, _virtualDimY, _virtualDimZ;
	T _min, _max, _mean, _minNonZero;
  double _dynamicRange;

	bool _isDelay;
	FILE *pFile; 

	
};

template <typename T>
volume3D<unsigned char>* volume3D<T>::generateUcharNormalized()
{
  updateMaxMin();
  volume3D<unsigned char>* vol = new volume3D<unsigned char>(_dimZ,_dimY,_dimX, 0);
  for(int k=0; k< _dimZ;k++)
    for(int j=0; j< _dimY;j++)
      for(int i=0; i< _dimX;i++)
      {
        vol->setVoxel(k,j,i, (unsigned char)( double(this->getVoxel(k,j,i)-_min)/double(_max - _min)*255));
      }

      return vol;
}


template <typename T>
volume3D<unsigned char>* volume3D<T>::generateUchar()
{
  volume3D<unsigned char>* vol = new volume3D<unsigned char>(_dimZ,_dimY,_dimX, 0);
  for(int k=0; k< _dimZ;k++)
    for(int j=0; j< _dimY;j++)
      for(int i=0; i< _dimX;i++)
  {
    vol->setVoxel(k,j,i, (unsigned char)(this->getVoxel(k,j,i)*255));
  }
  
  return vol;
}


template <typename T>
void volume3D<T>::normalize()
{
    for (unsigned long long i=0; i<_dimZ*_dimY*_dimX; i++)
      _data[i] = (T)double(_data[i] - _min)/double(_max - _min); 
}


template <typename T>
void volume3D<T>::normalize(T min, T max)
{
  
  for (unsigned long long i=0; i<_dimZ*_dimY*_dimX; i++)
  {
    if(_data[i]>=max)
      _data[i] = max;
    _data[i] = (_data[i] - min)/(max - min); 
  }

}


template <typename T>
void volume3D<T>::generateHistogram( int binNum/*=256*/ )
{
  _histo = new double[binNum];
  memset(_histo, 0, sizeof(double)*binNum);
  for (unsigned long long i=0; i<_dimZ*_dimY*_dimX; i++)
  {
    _histo[int((binNum-1)*double(_data[i]-_min)/double(_max-_min))] ++; 
  }
  //printf
  for(int i=0; i<binNum; i++)
    printf("index:%d = %f\n", i,_histo[i]);

}


template <typename T>
void volume3D<T>::calculateVolumeStatistic()
{
  updateMaxMin();
}


template<typename T>
volume3D<T>::volume3D(const char* filename, int z, int y, int x, bool delayLoad)
:_dimX(x),_dimY(y),_dimZ(z),_isDelay(delayLoad)
{

	_voxelByteSize = sizeof(T);
	_filename = string(filename);
	_data = 0;

	
	if(!_isDelay)
	{
		//fopen_s(&pFile, filename,"rb");
		pFile= fopen(filename, "rb");
		if (NULL == pFile) {
			cout<<"can't open file!"<<endl;
      exit(0);
			return;
		}
		_data = new T[_dimX*_dimY*_dimZ];
    if(!_data)
      return;
		memset( (void*)_data, _voxelByteSize*_dimX*_dimY*_dimZ, 0);
		fread( (void*)_data,_voxelByteSize, _dimX*_dimY*_dimZ, pFile);
	}
	else
	{
		pFile= fopen(filename, "rb");
		if (NULL == pFile) {
			cout<<"can't open file!"<<endl;\
			return;
		}
	}
}

template<typename T>
volume3D<T>::volume3D(T *data, int z, int y, int x)
:_dimX(x),_dimY(y),_dimZ(z)
{
	_voxelByteSize = sizeof(T);
	_data = data;
}

template<typename T>
volume3D<T>::volume3D(int z, int y, int x, int value)
:_dimX(x),_dimY(y),_dimZ(z)
{
	_voxelByteSize = sizeof(T);
	_data = new T[_dimX*_dimY*_dimZ];
	memset((void*)_data, value, _voxelByteSize*_dimX*_dimY*_dimZ);
}

template<typename T>
volume3D<T>::~volume3D()
{
	if(_data)
		free(_data);
}

//generater
template<typename T>
volume3D<T>* volume3D<T>::generateVolume16bitTo8bit()
{
	volume3D<T>* vol = new volume3D<T>(_dimZ, _dimY, _dimX, 1, 0);

	int value = 0;
	for(int k=0; k<_dimZ; k++)
		for(int j=0; j<_dimY; j++)
			for(int i=0; i<_dimX; i++)
			{
				value = getVoxelNN(double(k)/double(_dimZ) ,double(j)/double(_dimY)  ,double(i)/double(_dimX));
				value = int( double(value)/4096.0*256.0+0.5);
				vol->setVoxel(k, j, i, value);
			}


	return vol;

}

//TODO refactor this function remove the if else on _voxelByteSize;
template<typename T>
volume3D<T>* volume3D<T>::generateDownSampleVolume()
{
	volume3D<T>* vol = 0;

	int ddimX = _dimX/2; 
	int ddimY = _dimY/2;
	int ddimZ = _dimZ/2;
	float* tempf = new T[ddimX*ddimY*ddimZ]; //for more precision
	memset(tempf, 0, ddimX*ddimY*ddimZ*sizeof(float));
	T* temp = new T[ddimX*ddimY*ddimZ];
	memset(temp, 0, ddimX*ddimY*ddimZ*_voxelByteSize);

	for(int i=0; i<_dimX; i++)
		for(int j=0; j<_dimY; j++)
			for(int k=0; k<_dimZ; k++)
			{								
				tempf[k/2*ddimX*ddimY+ j/2*ddimY+ i/2]
				+= _data[k*_dimX*_dimY + j*_dimY + i];
			}	

	for(int i=0; i<ddimX*ddimY*ddimZ; i++)
	{
		((T*)(temp))[i] = (T)(tempf[i]/8.0);
	}

	free(tempf);

	vol = new volume3D<T>(temp, ddimZ, ddimY, ddimX);

	return vol;
}

template<typename T>
volume3D<T>* volume3D<T>::generateDownSampleVolumeNN()
{
	int ddimZ= _dimZ/2;
	int ddimY= _dimY/2;
	int ddimX= _dimX/2;
	volume3D<T>* vol = new volume3D<T>(ddimZ, ddimY, ddimX, _voxelByteSize, 0);

	for(int k=0; k<ddimZ; k++)
		for(int j=0; j<ddimY; j++)
			for(int i=0; i<ddimX; i++)
				vol->setVoxel(k, j, i, getVoxelNN(double(k)/double(ddimZ) ,double(j)/double(ddimY)  ,double(i)/double(ddimX)));


	return vol;
}

template<typename T>
volume3D<T>* volume3D<T>::generateDownSampleVolumeWithBoundaryNN()
{
	int ddimZ= _dimZ/2+1;
	int ddimY= _dimY/2+1;
	int ddimX= _dimX/2+1;
	volume3D<T>* vol = new volume3D<T>(ddimZ, ddimY, ddimX, _voxelByteSize, 0);
	for(int k=0; k<ddimZ; k++)
		for(int j=0; j<ddimY; j++)
			for(int i=0; i<ddimX; i++)
				vol->setVoxel(k, j, i, getVoxelNN(double(k)/double(ddimZ-1) ,double(j)/double(ddimY-1)  ,double(i)/double(ddimX-1)));

	//vol->writeToFile("test129X129X129.raw");
	return vol;

}

template<typename T>
volume3D<T>* volume3D<T>::generateUpSampleVolume()
{
	int ddimZ= _dimZ*2;
	int ddimY= _dimY*2;
	int ddimX= _dimX*2;
	volume3D<T>* vol = new volume3D<T>(ddimZ, ddimY, ddimX, _voxelByteSize, 0);

	for(int k=0; k<ddimZ; k++)
		for(int j=0; j<ddimY; j++)
			for(int i=0; i<ddimX; i++)
				vol->setVoxel(k, j, i, getVoxel(double(k)/double(ddimZ-1) ,double(j)/double(ddimY-1)  ,double(i)/double(ddimX-1)));
	return vol;
}

template<typename T>
volume3D<T>* volume3D<T>::generateSubVolume(int minZ, int maxZ,
													int minY, int maxY,
													int minX, int maxX)
{
	if(minZ>_dimZ-1 || minY>_dimY-1 || minX>_dimX-1)
		return 0;

	int ddimX = maxX-minX+1;
	int ddimY = maxY-minY+1;
	int ddimZ = maxZ-minZ+1;

	volume3D<T>* vol = 0;
	if(!_isDelay)
	{
		vol = new volume3D<T>(ddimZ, ddimY, ddimX, _voxelByteSize,0);
		for(int k=minZ; k<=maxZ; k++)
			for(int j=minY; j<=maxY; j++)
				for(int i=minX; i<=maxX; i++)
					vol->setVoxel(k-minZ,j-minY,i-minX, getVoxel(k,j,i));
	}
	else //Need rewrite TODO
	{
		T* _temp = new T[ddimZ*ddimY*ddimZ];
	
		//calculate byte to seek
		long int index = 0;
		long int indexOld = 0;
		long int offset = 0;

		for(int k=minZ; k<=maxZ; k++)
			for(int j=minY; j<=maxY; j++)
				for(int i=minX; i<=maxX; i++)
				{
					index = _dimY*_dimX*k + _dimX*j + i;
					fseek ( pFile , index , SEEK_SET );

					fread(((T*)_temp+offset), _voxelByteSize, 1, pFile);
					offset++;
					indexOld = index;
				}
	
		vol = new volume3D<T>(_temp, (maxZ-minZ+1),(maxY-minY+1),(maxX-minX+1),_voxelByteSize);
				
	}
	return vol;
}

//TODO
template<typename T>
volume3D<T>* volume3D<T>::generateSubVolume(double minZ, double maxZ,
													double minY, double maxY,
													double minX, double maxX)
{
	volume3D<T>* vol = 0;
	return vol;
}

//TODO
template<typename T>
volume3D<T>* volume3D<T>::generateOctBlock(int z, int y, int x, bool boundary)
{
	volume3D<T>* vol = 0;
	


	return vol;
}

//volume modifer
template<typename T>
bool volume3D<T>::addSubVolume(volume3D<T>* vol, int z, int y, int x)
{
	int ddimX = vol->getDimX();
	int ddimY = vol->getDimY();
	int ddimZ = vol->getDimZ();

	assert(x>=0 && y>=0 && z>=0);

	if(z+ddimX>_dimX && y+ddimY>_dimY && z+ddimZ> _dimZ)
		return false;	
	for(int i=0; i<ddimX; i++)
		for(int j=0; j<ddimY; j++)
			for(int k=0; k<ddimZ; k++)
				this->setVoxel(z+k, y+j, x+i, vol->getVoxel(k,j,i));
	return true;
}

template<typename T>
void volume3D<T>::addSphereRegion(int cz, int cy, int cx, int radius, T value)
{
	int ddimX = getDimX();
	int ddimY = getDimY();
	int ddimZ = getDimZ();

	for(int i=0; i<ddimX; i++)
		for(int j=0; j<ddimY; j++)
			for(int k=0; k<ddimZ; k++)
			{
				if(	(i-cx)*(i-cx) + (j-cy)*(j-cy) + (k-cz)*(k-cz) <= radius*radius )
					setVoxel(k,j,i, value);
			}
}


	//slice generater/modifer
template<typename T>
slice2D<T>* volume3D<T>::generateSliceX(int index, int minY, int maxY, int minX, int maxX)
{
	slice2D<T>* slice = new slice2D<T>(_voxelByteSize,(maxY-minY+1),(maxX-minX+1));

	for(int j=0; j<=(maxY-minY); j++)
		for(int i=0; i<=(maxX-minX); i++)
			slice->setPixel(j,i, getVoxel(minY+j, minX+i,index));

	return slice;
}

template<typename T>
slice2D<T>* volume3D<T>::generateSliceY(int index, int minY, int maxY, int minX, int maxX)
{
	slice2D<T>* slice = new slice2D<T>(_voxelByteSize,(maxY-minY+1),(maxX-minX+1));

	for(int j=minY; j<=maxY; j++)
		for(int i=minX; i<=maxX; i++)
			slice->setPixel(j-minY,i-minX, getVoxel(j,index,i));

	return slice;
}

template<typename T>
slice2D<T>* volume3D<T>::generateSliceZ(int index, int minY, int maxY, int minX, int maxX)
{
	slice2D<T>* slice = new slice2D<T>(_voxelByteSize,(maxY-minY+1),(maxX-minX+1));

	for(int j=minY; j<=maxY; j++)
		for(int i=minX; i<=maxX; i++)
			slice->setPixel(j-minY,i-minX, getVoxel(index,j,i));

	return slice;
}

//update slice
template<typename T>
void volume3D<T>::updateSliceX(slice2D<T>* slice, int index, int minY, int maxY, int minX, int maxX)
{
	for(int j=0; j<=maxY-minY; j++)
		for(int i=0; i<=maxX-minX; i++)
			setVoxel(j+minY,i+minX,index, slice->getPixelBilinear(double(j)/double(maxY),double(i)/double(maxX)));
}

template<typename T>
void volume3D<T>::updateSliceY(slice2D<T>* slice, int index, int minY, int maxY, int minX, int maxX)
{
	for(int j=0; j<=maxY-minY; j++)
		for(int i=0; i<=maxX-minX; i++)
			setVoxel(j+minY,index,i+minX, slice->getPixelBilinear(double(j)/double(maxY),double(i)/double(maxX)));
}

template<typename T>
void volume3D<T>::updateSliceZ(slice2D<T>* slice, int index, int minY, int maxY, int minX, int maxX)
{
	for(int j=0; j<=maxY-minY; j++)
		for(int i=0; i<=maxX-minX; i++)
			setVoxel(index,j+minY,i+minX, slice->getPixelBilinear(double(j)/double(maxY),double(i)/double(maxX)));
}


//TODO - 10/17/2010 => this function is not a good design
// updated to new one - 11/10/2010
template<typename T>
void  volume3D<T>::updateBoundaryToUpperLevelBlock(volume3D<T>* upperLevel, int zp, int zn, int yp, int yn, int xp, int xn) //x:0,1 y:0,1 z:0,1 
{



}

//accesser
template<typename T>
int volume3D<T>::getVoxelByteSize()
{
	return _voxelByteSize;
}

template<typename T>
T volume3D<T>::getVoxel(int z, int y, int x) //from 0 -> n-1
{
	if(x<0 || x>_dimX-1|| y<0 || y>_dimY-1 || z<0 || z>_dimZ-1)
		return 0;

	return ((T*)(_data))[ _dimX*_dimY*(z) + _dimX*y + x ];

	return 0;

}

template<typename T>
T volume3D<T>::getVoxelNN(double fz, double fy, double fx)
{
	assert(fz>=0 && fz<=1);
	assert(fy>=0 && fy<=1);
	assert(fx>=0 && fx<=1);

	int x = (int)floor((_dimX-1)*fx+0.5);
	int y = (int)floor((_dimY-1)*fy+0.5);
	int z = (int)floor((_dimZ-1)*fz+0.5);

	assert(x<_dimX);
	assert(y<_dimY);
	assert(z<_dimZ);

	return ((T*)(_data))[ _dimX*_dimY*(z) + _dimX*y + x ];

	return 0;
}


inline float lerp(float a, float b, float t)
{
	return (a*t+b*(1-t));
}

template<typename T>
T texture3D(volume3D<T>* vol, double indexZf, double indexYf, double indexXf, int sizeZ, int sizeY, int sizeX)
{
	assert(indexZf>=0 && indexZf<=1);
	assert(indexYf>=0 && indexYf<=1);
	assert(indexXf>=0 && indexXf<=1);
	unsigned int value=0;
	//sizeZ--;
	//sizeY--;
	//sizeX--;
	int indexZ = int( indexZf*(sizeZ-1));
	int indexY = int( indexYf*(sizeY-1));
	int indexX = int( indexXf*(sizeX-1));

	float offsetZ = float(indexZf*(sizeZ-1) - indexZ);
	float offsetY = float( indexYf*(sizeY-1) - indexY);
	float offsetX = float(indexXf*(sizeX-1) - indexX);

	//http://en.wikipedia.org/wiki/Trilinear_interpolation
	float v000, v001,v011, v010, v100, v101,v111, v110;
	float v00, v01, v11, v10;
	float v0, v1;
	v000 = (float)vol->getVoxel(indexZ+0, indexY+0, indexX+0);
	v001 = (float)vol->getVoxel(indexZ+0, indexY+0, indexX+1);
	v011 = (float)vol->getVoxel(indexZ+0, indexY+1, indexX+1);
	v010 = (float)vol->getVoxel(indexZ+0, indexY+1, indexX+0);
	v100 = (float)vol->getVoxel(indexZ+1, indexY+0, indexX+0);
	v101 = (float)vol->getVoxel(indexZ+1, indexY+0, indexX+1);
	v111 = (float)vol->getVoxel(indexZ+1, indexY+1, indexX+1);
	v110 = (float)vol->getVoxel(indexZ+1, indexY+1, indexX+0);

	v00 = lerp(v000, v100, 1-offsetZ); 
	v01 = lerp(v001, v101, 1-offsetZ);
	v11 = lerp(v011, v111, 1-offsetZ);
	v10 = lerp(v010, v110, 1-offsetZ);

	v0 = lerp(v00, v10, 1-offsetY); 
	v1 = lerp(v01, v11, 1-offsetY);

	value =(unsigned int)(lerp(v0, v1, 1-offsetX)+0.5);

	//debug
	//std::cout<<value<<endl;

	return value;
}

template<typename T>
T volume3D<T>::getVoxel(double z, double y, double x)
{
	return texture3D(this, z, y, x, _dimZ, _dimY, _dimX); 
}
	
template<typename T>
void volume3D<T>::generateVirtualDim(int blockSize) //padding dimension
{
		int x,y,z;
		if(getDimX()%blockSize==0)
			x=getDimX()/blockSize;
		else
			x=(getDimX()/blockSize +1);

		if(getDimY()%blockSize==0)
			y=getDimY()/blockSize;
		else
			y=(getDimY()/blockSize +1);

		if(getDimZ()%blockSize==0)
			z=getDimZ()/blockSize;
		else
			z=(getDimZ()/blockSize +1);

		int temp=((x>y?x:y)>(x>z?x:z))?(x>y?x:y):(x>z?x:z);
		int maxBlockDim = 1;
	    while (maxBlockDim < temp)
			maxBlockDim <<= 1;
		_virtualDimX=_virtualDimY=_virtualDimZ=maxBlockDim*blockSize;

}

//modifier

template<typename T>
bool volume3D<T>::setVoxel(int z, int y, int x, T value)
{
	if(x<0 || x>_dimX-1|| y<0 || y>_dimY-1 || z<0 || z>_dimZ-1)
		return false;

	((T*)(_data))[ _dimX*_dimY*(z) + _dimX*y + x ] = value;

	return true;

}

//status
template<typename T>
void volume3D<T>::updateMaxMin()
{
	T value = 0;

  _minNonZero = std::numeric_limits<T>::max();
	_min = std::numeric_limits<T>::max();
	_max = std::numeric_limits<T>::min();
		for(int k=0; k<_dimZ; k++)
			for(int j=0; j<_dimY; j++)
				for(int i=0; i<_dimX; i++)
				{
					value = _data[ _dimX*_dimY*(k) + _dimX*j + i ];
					if (value<_min)
						_min = value;
					if(value >_max)
						_max = value;
          if(abs(value) < _minNonZero && value != 0.0)
            _minNonZero = abs(value);
				}

  _dynamicRange = (_max - _min) / _minNonZero;

}

template<typename T>
void volume3D<T>::calculateMean()
{
	double value = 0;
	double sum = 0;
	double count = 0;

	for(int k=0; k<_dimZ; k++)
		for(int j=0; j<_dimY; j++)
			for(int i=0; i<_dimX; i++)
			{
				sum += (double)(_data[ _dimX*_dimY*(k) + _dimX*j + i ]);
				count++;
			}
	_mean = (T)(sum/count);
}

//debug
template<typename T>
std::string  volume3D<T>::number2String(int num)
{
	std::ostringstream stm;
	stm << num;

	return std::string(stm.str());

}
template<typename T>
bool volume3D<T>::writeToFile(const char* filename, const char* path)
{
	FILE *pFile; 

	std::string fpath = std::string(path);
	std::string fname=std::string(filename);
	fname = fpath + fname;
	fname += std::string("-zyx-");
	fname += (number2String(_dimZ)+std::string("X")+number2String(_dimY)+std::string("X")+number2String(_dimX));
	fname += std::string(".raw");

	pFile= fopen(fname.c_str(), "wb");
	if (NULL == pFile) {
		cout<<"can't open file!"<<endl;
		return false;
	}

	int size = fwrite((void*)_data, sizeof(T), _dimX*_dimY*_dimZ, pFile);
	if (size !=  _dimX*_dimY*_dimZ)
	{
		cout << "file Write Error"<< endl;
		return false;
	}
	cout <<"write:"<<fname.c_str()<< endl;
	return true;
}

#endif
