#include <assert.h>
#include <cstdio>
#include <fstream>

#include "RayCaster.h"
#include "RenderUtility/GLframebufferObject.h"
#include "RenderUtility/Vectors.h"
#include "PortableTimer.h"
#define HISTO_Y 200

RayCaster::RayCaster()
:_O(NULL),
_Dir(NULL),
_fbo(NULL),
_shadowTex(NULL),
_shadowRes(1),
_oldRot(MATRIX4<float>()),
_transVec(VECTOR3<float>())
{

}

RayCaster::~RayCaster()
{
	if(texBlockPool)
		delete texBlockPool;
	if(texNodePool3D)
		delete texNodePool3D;	

}
void RayCaster::ReLoadShader()
{
   _simpleProg.MakeProgramFromString(
     GLSLProgram::LoadShaderToString("ScreenAlignQuad_Vert.glsl"), 
     GLSLProgram::LoadShaderToString("Raycast_ScreenAlign_Frag.glsl"));

   _renderTo3Dtex.MakeProgramFromString(
     GLSLProgram::LoadShaderToString("shadowVolume_Vert.glsl"), 
     GLSLProgram::LoadShaderToString("shadowVolume_Frag.glsl"));

  //_testProgram.MakeProgramFromString(
     //GLSLProgram::LoadShaderToString("shadowVolume_Vert.glsl"), 
     //GLSLProgram::LoadShaderToString("TestFrag.glsl"));

}

void RayCaster::Init(int width, int height)
{
  printf("Init Raycaster");

  _width = width;
  _height = height;
  _aspectRatio = float(width)/float(height);  

  glInitStatus status;
  GL::InitGLStatus(status); //enable unaligned texture uploading

  //create default glsl program
  ReLoadShader();

  arcBall = new ArcBall(width, height);

  GL::CheckErrors();
}

void RayCaster::Resize(int width, int height)
{
  _width = width;
  _height = height;
  _aspectRatio = float(width)/float(height);  
}

bool RayCaster::LoadShader(std::string* VertString  ,std::string* FragString)
{
  bool isOK = _prog.MakeProgramFromString(VertString, FragString);
  return isOK;//better for debug - set break point

}
void transposeMatrixvf( float *input,  float *output)
{
	output[0]=input[0]; output[1]=input[4]; output[2]=input[8];output[3]=input[12];
	output[4]=input[1]; output[5]=input[5]; output[6]=input[9];output[7]=input[13];
	output[8]=input[2]; output[9]=input[6]; output[10]=input[10];output[11]=input[14];
	output[12]=input[3]; output[13]=input[7]; output[14]=input[11];output[15]=input[15];
}
void RayCaster::Render()
{

  glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat invRotationMatrix[16];
  MATRIX4<float> Rot(arcBall->GetRotation().array);
  _newRot=_oldRot*Rot;

  transposeMatrixvf(_newRot.array, invRotationMatrix);

  GL::SetupOrthogonalView(0,0, _width, _height, -1,1,-1,1);

	_simpleProg.BeginProgram();
	//define geometry ¨C full screen quad (depth at -1.0 : view direction is always with negative z-axis in OpenGL!)
  _newTransVec = _transVec + VECTOR3<float>(arcBall->GetTranslationVec());

	_simpleProg.setUniformMatf("rotMat4", invRotationMatrix);
  _simpleProg.setUniform3f("cameraO", _O);
  _simpleProg.setUniform3f("cameraDir", _Dir);
  _simpleProg.setUniform3f("cameraUp", _Up);

  _simpleProg.setUniform1f("stepsize", _paraS.stepsize);
	_simpleProg.setUniform1f("aspectRatio",_aspectRatio);
  _simpleProg.setUniform1f("insideView", (float)_paraS.cameraMode);
  _simpleProg.setUniform3f("translateVec3", &_newTransVec.x );

	_simpleProg.setUniform1f("sizeX", float(_sizeX));
	_simpleProg.setUniform1f("sizeY", float(_sizeY));
	_simpleProg.setUniform1f("sizeZ", float(_sizeZ));
  _simpleProg.setUniform1f("renderMode", float(_paraS.renderMode));
  _simpleProg.setUniform1f("isPreIntegration", float(_paraS.preIntegration));

//don't use GL_TEXTURE0 !!!! WHY????
  _simpleProg.BindTexture(1,GL_TEXTURE_1D,_CDFlut,"CDFtranf");  
	_simpleProg.BindTexture(2,GL_TEXTURE_1D,_lut,"tranf");
  _simpleProg.BindTexture(3, texBlockPool->GetTextureType(), texBlockPool->GetTextureID(),"blockPool");
  _simpleProg.BindTexture(4,GL_TEXTURE_2D,_preInt2D,"preInt2D");

	GL::DrawFullScreenQuadNegativeOneToOne();
	
	_simpleProg.EndProgram();
  GL::CheckErrors();
}

void RayCaster::RenderShadow()
{
  if(_fbo==0)
    return;

  glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat invRotationMatrix[16];
  MATRIX4<float> Rot(arcBall->GetRotation().array);
  _newRot=_oldRot*Rot;

  transposeMatrixvf(_newRot.array, invRotationMatrix);

  GL::SetupOrthogonalView(0,0, _width, _height, -1,1,-1,1);

  _newTransVec = _transVec + VECTOR3<float>(arcBall->GetTranslationVec());

	_simpleProg.BeginProgram();
	//define geometry ¨C full screen quad (depth at -1.0 : view direction is always with negative z-axis in OpenGL!)
	_simpleProg.setUniformMatf("rotMat4", invRotationMatrix);
  _simpleProg.setUniform3f("cameraO", _O);
  _simpleProg.setUniform3f("cameraDir", _Dir);
  _simpleProg.setUniform3f("cameraUp", _Up);

  _simpleProg.setUniform1f("stepsize", _paraS.stepsize);
	_simpleProg.setUniform1f("aspectRatio",_aspectRatio);
  _simpleProg.setUniform1f("insideView", (float)_paraS.cameraMode);
  _simpleProg.setUniform3f("translateVec3", &_newTransVec.x);

	_simpleProg.setUniform1f("sizeX", float(_sizeX));
	_simpleProg.setUniform1f("sizeY", float(_sizeY));
	_simpleProg.setUniform1f("sizeZ", float(_sizeZ));

  _simpleProg.setUniform1f("renderMode", float(_paraS.renderMode));
  _simpleProg.setUniform1f("isPreIntegration", float(_paraS.preIntegration));


  _simpleProg.BindTexture(1,GL_TEXTURE_1D,_CDFlut,"CDFtranf");
	_simpleProg.BindTexture(2,GL_TEXTURE_1D,_lut,"tranf");

  _simpleProg.BindTexture(3, _shadowTex->GetTextureType(), _shadowTex->GetTextureID(),"shadowVolume");
  _simpleProg.BindTexture(4, texBlockPool->GetTextureType(), texBlockPool->GetTextureID(),"blockPool");
  _simpleProg.BindTexture(5,GL_TEXTURE_2D,_preInt2D,"preInt2D");

//printf("_CDFlut:%d", _CDFlut);

	GL::DrawFullScreenQuadNegativeOneToOne();
	
	_simpleProg.EndProgram();
  GL::CheckErrors();
}

void drawSlice(float z)
{
    glBegin(GL_QUADS);
    glTexCoord3f(0.0f, 0.0f, z); glVertex2f(-1.0f, -1.0f);
    glTexCoord3f(1.0f, 0.0f, z); glVertex2f(1.0f, -1.0f);
    glTexCoord3f(1.0f, 1.0f, z); glVertex2f(1.0f, 1.0f);
    glTexCoord3f(0.0f, 1.0f, z); glVertex2f(-1.0f, 1.0f);
    glEnd();
}

void RayCaster::TestRender()
{
  _testProgram.BeginProgram();
  _testProgram.BindTexture(1,GL_TEXTURE_1D,_CDFlut,"CDFtranf");
	GL::DrawFullScreenQuadNegativeOneToOne();
  _testProgram.EndProgram();
  
}

void RayCaster::UpdateShadowVolume()
{
  if(_fbo == NULL)
  {
    //create Texture
    //_shadowTex = new GLTexture(_sizeX/_shadowRes,_sizeY/_shadowRes,_sizeZ/_shadowRes,GL_RED, GL_R16F);
    _shadowTex = new GLTexture(_sizeX/_shadowRes,_sizeY/_shadowRes,_sizeZ/_shadowRes,GL_LUMINANCE, GL_INTENSITY);
    _shadowTex->LoadToGPU();

    _fbo = new FramebufferObject();
    //create PBO
    _fbo->Bind();
    _fbo->AttachTexture(GL_TEXTURE_3D, _shadowTex->GetTextureID(), GL_COLOR_ATTACHMENT0_EXT,0,0);
    _fbo->IsValid();
    _fbo->Disable();

  }

  if(_fbo)
  {
    _fbo->Bind();
     //update
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);

    glViewport(0, 0, _sizeX/_shadowRes, _sizeY/_shadowRes); //same as the x y of the target
    glDisable(GL_DEPTH_TEST);

    _renderTo3Dtex.BeginProgram();
    _renderTo3Dtex.BindTexture(1, GL_TEXTURE_1D, _lut, "tf");
    _renderTo3Dtex.BindTexture(3, GL_TEXTURE_3D, texBlockPool->GetTextureID(), "volume");

    for(int z=0; z<(_sizeZ/_shadowRes); z++) {
        // attach texture slice to FBO
        _fbo->AttachTexture(GL_TEXTURE_3D, _shadowTex->GetTextureID(), GL_COLOR_ATTACHMENT0_EXT, 0, z);
        // render
        drawSlice((z + 0.5f) / (float) (_sizeZ/_shadowRes));
    }

    _renderTo3Dtex.EndProgram();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    _fbo->Disable();
  }

}

void RayCaster::SetShadingParameters(parameterShading &paraS)
{
  _paraS = paraS;
  if(_paraS.stepsize < 0.0001f)
    _paraS.stepsize = 0.001f;


}

void RayCaster::LoadRendererSetting(const char* filename)
{
  std::ifstream infile(filename);
  if(!infile) {
    std::cerr << filename << ": rendering status file not found!" << std::endl;
    exit(0);
    return;
  }
  infile>>_oldRot;
  infile>>_transVec;
  infile>>_paraS.stepsize;//pass a pointer instead of the struct
  

}

void RayCaster::SaveRendererSetting(const char* filename )
{
  std::ofstream outfile(filename);
  outfile<<_newRot; outfile<<"\n";
  outfile<<_newTransVec; outfile<<"\n";
  outfile<<_paraS.stepsize;
  outfile.close();
}

bool RayCaster::LoadVolumeBuffer( void* buffer, int z, int y, int x, dataType type )
{
  if(type == Uchar)
    texBlockPool = new GLTexture(x, y, z, GL_LUMINANCE, GL_INTENSITY);
  else if(type == Float)
    texBlockPool = new GLTexture(x, y, z, GL_LUMINANCE, GL_INTENSITY, GL_FLOAT, GL_LINEAR);
  else if(type == Int)
    texBlockPool = new GLTexture(x, y, z, GL_LUMINANCE, GL_INTENSITY, GL_INT);
  else if (type == Uint)
    texBlockPool = new GLTexture(x, y, z, GL_LUMINANCE, GL_INTENSITY, GL_UNSIGNED_INT);

  texBlockPool->LoadToGPU(buffer);

  _sizeX = x; _sizeY = y; _sizeZ = z;

  return true;
}
