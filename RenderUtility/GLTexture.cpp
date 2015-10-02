
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <assert.h>

#include "GL.h"
#include "GLTexture.h"
#include "GLBufferObject.h"

//GLTexture::GLTexture() {}

GLTexture::GLTexture(int width, int height, int depth, GLenum elementFormat, GLint  internalFormat, GLenum dataType, GLenum filterType, GLenum borderType)
{
  _elementType = dataType;
  _data = NULL;
  InitTexture(width, height, depth, elementFormat, internalFormat, filterType, borderType);
}

GLTexture::~GLTexture()
{
  if(_data)
    free(_data);
  glDeleteTextures(1, &_tex);
}

void GLTexture::InitTexture(int width, int height, int depth, GLenum elementFormat, 
			    GLint  internalFormat, GLenum filterType, GLenum borderType)
{
  _dim[0] = width;
  _dim[1] = height;
  _dim[2] = depth;

  _borderType = borderType;
  _filterType = filterType;
  _elementFormat = elementFormat;
  _internalFormat =  internalFormat;

  glGenTextures(1,	 &_tex);

  if(height==0 && depth==0)
  {
    _textureType = GL_TEXTURE_1D;
    this->Bind();
    glTexParameteri(_textureType,GL_TEXTURE_WRAP_S, _borderType);
    glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, _filterType);
    glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, _filterType);

  }
  else if(depth==0)
  {
    _textureType = GL_TEXTURE_2D;
    this->Bind();
    glTexParameteri(_textureType,GL_TEXTURE_WRAP_S, _borderType);
    glTexParameteri(_textureType,GL_TEXTURE_WRAP_T, _borderType);
    glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, _filterType);
    glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, _filterType);
  }
  else
  {
    _textureType = GL_TEXTURE_3D;
    this->Bind();
    glTexParameteri(_textureType,GL_TEXTURE_WRAP_S, _borderType);
    glTexParameteri(_textureType,GL_TEXTURE_WRAP_T, _borderType);
    glTexParameteri(_textureType,GL_TEXTURE_WRAP_R, _borderType);
    glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, _filterType);
    glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, _filterType);
  }
}

//==============================================================
//Accessors
GLuint GLTexture::GetTextureID()
{
  return _tex;
}

GLuint GLTexture::GetHeight()
{
  return _dim[0];
}
GLuint GLTexture::GetWidth()
{
  return _dim[1];
}

GLuint GLTexture::GetDepth()
{
  return _dim[2];
}

GLenum GLTexture::GetTextureType()
{
  return   _textureType;
}

//Operations
void  GLTexture::Bind()
{
  glBindTexture(_textureType, _tex);
}

//modifier
void GLTexture::SetFilterType(GLenum filter)
{
  _filterType = filter;
  this->Bind();
  //glTexParameteri(_textureType,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(_textureType,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  //glTexParameteri(_textureType,GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, _filterType);
  glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, _filterType);	
}


//==============================================================
// Data transfer

void GLTexture::LoadToGPU(void* data)
{
  if(_textureType == GL_TEXTURE_1D)
    glTexImage1D(_textureType,  0,  _internalFormat,  _dim[0], 0,  _elementFormat, _elementType, data);
  else if(_textureType == GL_TEXTURE_2D)
    glTexImage2D(_textureType, 0, _internalFormat, _dim[0], _dim[1], 0, _elementFormat, _elementType, data);
  else
    glTexImage3D(_textureType,0,_internalFormat, _dim[0], _dim[1], _dim[2], 0, _elementFormat, _elementType, data);
}

void GLTexture::LoadToGPU()
{
  if(_textureType == GL_TEXTURE_1D)
    glTexImage1D(_textureType,  0,  _internalFormat,  _dim[0], 0,  _elementFormat, _elementType, _data);
  else if(_textureType == GL_TEXTURE_2D)
    glTexImage2D(_textureType, 0, _internalFormat, _dim[0], _dim[1], 0, _elementFormat, _elementType, _data);
  else
    glTexImage3D(_textureType,0,_internalFormat, _dim[0], _dim[1], _dim[2], 0, _elementFormat, _elementType, _data);

  GL::CheckErrors();
}

void GLTexture::LoadToGPUWithGLBuffer()
{
  //allocate GL texture in GPU
  glTexImage3D(_textureType,0,_internalFormat, _dim[0], _dim[1], _dim[2], 0, _elementFormat, _elementType, NULL);
  GL::CheckErrors();

  //bind PBO
  GLBufferObject GLbuffer(GL_PIXEL_UNPACK_BUFFER, _dataSize );
  GLbuffer.Bind();
  GLbuffer.BufferDataStaticDraw(NULL, _dataSize);  //here GPU allocate memory for the buffer

  void* vmemBuffer = GLbuffer.MapBuffer(GL_WRITE_ONLY);	 
  assert(vmemBuffer);
  memcpy(vmemBuffer, _data, _dataSize);
  GLbuffer.UnMapBuffer();

  glTexSubImage3D(_textureType,0,0,0,0,_dim[0], _dim[1], _dim[2],_elementFormat, _elementType, NULL)   ;

  GLbuffer.BindEmpty();
  //glDeleteBuffer will release the memory

	

}

//================================================================
//subTexture
void GLTexture::SubloadToGPU(int offsetX, int offsetY, int offsetZ, int sizeX, int sizeY, int sizeZ, void* data, GLenum elementType)
{
  Bind(); //!!!!!! Important
  if(_textureType == GL_TEXTURE_1D)
    glTexSubImage1D(_textureType,  0,  offsetX, sizeX,  _elementFormat, elementType, data);
  else if(_textureType == GL_TEXTURE_2D)
    glTexSubImage2D(_textureType, 0, offsetX, offsetY, sizeX, sizeY,  _elementFormat, elementType, data);
  else
    glTexSubImage3D(_textureType, 0, offsetX, offsetY, offsetZ, sizeX, sizeY, sizeZ,_elementFormat, elementType, data);

  //GL::CheckErrors();
}

void GLTexture::preAllocateGLPBO(GLsizei bufferSize, GLenum usage=GL_STREAM_DRAW)
{
  if(_isBufferAllocated)
  {
    delete _GLbuffer;    //need resize the PBO
  }

  _GLbuffer = new GLBufferObject(GL_PIXEL_UNPACK_BUFFER, bufferSize);
  _GLbuffer->Bind();
  _GLbuffer->BufferData(NULL, usage);
}

//the offset is wrong in this function
//TODO
void GLTexture::subloadToGPUWithGLBuffer(int offsetX, int offsetY, int offsetZ, int sizeX, int sizeY, int sizeZ, void* data)
{
  if(_isBufferAllocated)
  {
    _GLbuffer->Bind();
    void* vmemBuffer = _GLbuffer->MapBuffer(GL_STREAM_DRAW); //assume update every frame	 
    assert(vmemBuffer);
    memcpy(vmemBuffer, _data, _dataSize);
    _GLbuffer->UnMapBuffer();

    glTexSubImage3D(_textureType,0,offsetX,offsetY,offsetZ,_dim[0], _dim[1], _dim[2],_elementFormat, _elementType, NULL)   ;

    _GLbuffer->BindEmpty();
  }
  else	   //if buffer is not allocated assume only use static PBO
  {
    _GLbuffer = new GLBufferObject(GL_PIXEL_UNPACK_BUFFER, sizeX*sizeY*sizeZ*_elementByteSize);
    _GLbuffer->Bind();
    _GLbuffer->BufferData(NULL, GL_WRITE_ONLY);
    void* vmemBuffer = _GLbuffer->MapBuffer(GL_WRITE_ONLY);	 
    assert(vmemBuffer);
    memcpy(vmemBuffer, _data, _dataSize);
    _GLbuffer->UnMapBuffer();

    glTexSubImage3D(_textureType,0,0,0,0,_dim[0], _dim[1], _dim[2],_elementFormat, _elementType, NULL)   ;

    _GLbuffer->BindEmpty();
  }

}


//numPerElement is the number of elementType in the internal format
bool GLTexture::ReadTextureFromFile(const char* filename, GLenum elementType, int channelNum)
{
  //over write the original elementType
  _elementType = elementType;
  _fileName = std::string(filename);
  _fileType = _fileName.substr(_fileName.find_last_of('.')+1);

  //determin element byte size  
  switch(elementType)
  {
  case GL_UNSIGNED_BYTE:
  case GL_BYTE:
    _elementByteSize = sizeof(char)*channelNum;	
    break;
  case GL_UNSIGNED_SHORT:
  case GL_SHORT:
    _elementByteSize = sizeof(short)*channelNum;
    break;
  case GL_UNSIGNED_INT:
  case GL_INT:
    _elementByteSize = sizeof(int)*channelNum;
    break;
  case GL_FLOAT:
    _elementByteSize = sizeof(float)*channelNum;
    break;
  }

  //loading from file		 
  if( _fileType == std::string("raw") || _fileType == std::string("RAW"))
  {
    if( (_data = loadRawFile(_elementByteSize, &_dataSize)) == NULL)
    {
      std::cout<<"Read file Failed!"<<std::endl;
      return false;
    }
    else
      return true;
  }
  else if(_fileType == std::string("PPM"))
  {

  }

  std::cout<<"Texture file type unknown !"<<std::endl;

  return false;
}


//////////////////////////////////////
//private
void* GLTexture::loadRawFile(int elementByteSize, int *dataSize)
{
  FILE *pFile = fopen(_fileName.c_str(),"rb");
  if (NULL == pFile) {
    return 0;
  }

  int i=0;
  unsigned int size=1;
  for(i=0; i<3; i++)
  {
    if(_dim[i]!=0)
      size*=_dim[i];
  }
		
  void *pVolume = malloc(size*elementByteSize);
  memset(pVolume,0,size*elementByteSize);
	
  if(size != fread(pVolume,elementByteSize, size,pFile))
    return 0;
  fclose(pFile);
  *dataSize = elementByteSize*size;

  return pVolume;
}
