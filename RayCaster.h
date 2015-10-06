#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "RenderUtility/GL.h"
#include "RenderUtility/GLTexture.h"
#include "RenderUtility/GLSLProgram.h"
#include "RenderUtility/GLTextureBuffer.h"
#include "RenderUtility/GLframebufferObject.h"

#include "RenderUtility/ArcBall.h"
class FramebufferObject;

////////////////////////////Parameters//////////////////////////////
struct parameterShading
{
  //phong shading parameter
  float shininess;
  float diffuse;
  float specular;
  float lightPosition[4];
  //volume rendering
  float isoValue;
  float stepsize;
  float clippingX, clippingY, clippingZ;
  int tranferFunctionType;
  //temp
  int cameraMode;
  int renderMode;
  int showVol;
  int preIntegration;

  parameterShading()
  {
    stepsize = 0.002f;
    cameraMode = 1;
    renderMode = 0;//0 normal; 1 volume shadow
    showVol = 0;
    preIntegration = 0;
  }


};

struct parameterViewing
{
  //viewing matrix
  float* rotateMatrix;//16
  float* posVec;//3
  float* viewProjMatrix;//16
  //view position

};

struct rendererParameter
{
  int viewportX, viewportY;
  char filename[500];
  char fileType[500];
  int volDimX, volDimY, volDimZ;
  int volScaleX, volScaleY, volScaleZ;
  //virtual walk through
  char* pathFilename;
  int startPos[3];
};

enum dataType
{
  Uchar,
  Uint,
  Int,
  Float
};

////////////////////////////Parameters//////////////////////////////


class RayCaster
{
public:
  RayCaster();
  ~RayCaster();

  void Init(int width, int height);
  bool LoadShader(std::string* VertString  ,std::string* FragString);
  void LoadRendererSetting(const char*);
  void SaveRendererSetting(const char*);
  void ReLoadShader();

  void Render();
  void RenderShadow();
  void TestRender();
  void UpdateShadowVolume();
  void Resize(int width, int height);

  //Data Source
  void SetLookUpTable(unsigned int* table, size_t len);
  //
  void SetLookUpTableGLTex(GLuint tex){_lut = tex;}
  void SetCDFTableGLTex(GLuint tex){_CDFlut = tex;}
  void SetPreInt2DTableGLTex(GLuint tex){_preInt2D = tex;}

  //get
  int getViewportWidth(){ return _width;}
  int getViewportHeight(){ return _height;}

  //change renderer status
  //void SetViewingParameters(parameterViewing &paraV){_paraV = paraV;}
  void SetShadingParameters(parameterShading &paraS);//{_paraS = paraS;}
  void SetPathParameters(float *O, float *Dir, float *Up){_O = O; _Dir = Dir; _Up = Up;}

  bool LoadVolumeBuffer(void* buffer, int z, int y, int x, dataType type);

///////////////////////////////////////////////////////////////////////////
public:
  ArcBall *arcBall;
  //status for saving
  MATRIX4<float> _oldRot;
  MATRIX4<float> _newRot;
  VECTOR3<float> _transVec;
  VECTOR3<float> _newTransVec;

private:
  parameterViewing _paraV;
  parameterShading _paraS;
  float *_O; //3  camera at world space
  float *_Dir; //3 camera position in world space
  float *_Up; //the up direction of the camera

  //raycaster parameter
  int _width, _height;
  float _aspectRatio;

  //program
  GLSLProgram _prog;
  GLSLProgram _simpleProg;
  GLSLProgram _renderTo3Dtex;
  GLSLProgram _testProgram;

  //color table
  GLTexture *_texColorTable;
  GLuint _lut, _CDFlut, _preInt2D;

  //temps
  GLTexture *texNodePool3D;
  GLTexture *texBlockPool;

  float light_rotate[16];

private:
  //GLTexture *texFront, *texBack;
  unsigned int _blockPoolDim;
  unsigned int _nodePoolDim;
  unsigned int _blockSize;

  //volume size
  int _sizeX;
  int _sizeY;
  int _sizeZ;

  //volume shadow
  FramebufferObject* _fbo;
  GLTexture *_shadowTex;
  int _shadowRes;

};



#endif
