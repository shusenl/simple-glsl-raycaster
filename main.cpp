#include <math.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>

#include "RayCaster.h"
#include "TransferFunction.h"
#include "RenderUtility/Vectors.h"
#include "RenderUtility/volume3D.h"

using namespace std;

//lighting
float light_rotate[16] ={1, 0, 0, 0,
                 0, 1, 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1};


int window_id = 0;
float stepsize = 0.0081;
float var = 0.008;
float integralMethod = 1;
float isovalue = 0.2;
int showPacking = 0;
int viewMethod = 0;
int showTF = 0;

//=================================
RayCaster *raycaster = NULL;
TransferFunction *transferfunction = NULL;

//parameterViewing paraV;
parameterShading paraS;
float O[3] = {0,0,4};
float Dir[3] = {0,0,-1};
float Up[3] = {0,1,0};
int x = 0;

void display()
{
  //raycaster->SetViewingParameters(paraV);
  raycaster->SetShadingParameters(paraS);

  //get the ray direction from arcBall
  VECTOR3<float> dir(0,0,-1);
  dir = raycaster->arcBall->GetRotation()*dir;
  dir.normalize();
  Dir[0] = dir.x; Dir[1] = dir.y; Dir[2] = dir.z;
  //if set O Dir Up directly remove the arcBall code above
  raycaster->SetPathParameters(O, Dir, Up);


  if(x == 0)
  {
  if(paraS.renderMode == 0)
    raycaster->Render();
  else
    raycaster->RenderShadow();

  if(showTF == 1)
    transferfunction->Render();
  }
  else
  {
    raycaster->TestRender();
  }
  glutSwapBuffers();
}

void idle(void)
{
  glutSetWindow(window_id);
  glutPostRedisplay();
}

void reshape (int w, int h)
{
  raycaster->Resize(w,h);
  raycaster->arcBall->SetWindowSize(w,h);
}

void keyboard(unsigned char key, int x, int y)
{
  VECTOR3<float> dir(Dir[0],Dir[1],Dir[2]);
  VECTOR3<float> up(Up[0],Up[1],Up[2]);
  VECTOR3<float> left = dir%up;
  left.normalize();
  //dir = raycaster->arcBall->GetRotation()*dir;
  switch(key)
  {
    case '27':
      exit(0);
    break;
    case 't':
      if(showTF==0)
        showTF=1;
      else
        showTF=0;
        break;
    case 'r':
      raycaster->ReLoadShader();
      break;
    case '-':
      paraS.stepsize -= 0.002;
      break;
    case '=':
      paraS.stepsize += 0.002;
      break;
    case 'S':
      printf("save transfer function!\n");
      transferfunction->SavetoFile("TransparentSytle1");
      raycaster->SaveRendererSetting("RenderStatus");
      break;
    case 'c':
      if(paraS.cameraMode == 0) paraS.cameraMode = 1;
      else paraS.cameraMode = 0;
    case 'w': //advance
      //printf("Dir: %f %f %f\n", dir.x, dir.y, dir.z);
      O[0] = O[0]+Dir[0]*0.1;
      O[1] = O[1]+Dir[1]*0.1;
      O[2] = O[2]+Dir[2]*0.1;
      break;
    case 's': //backward
      //printf("Dir: %f %f %f\n", dir.x, dir.y, dir.z);
      O[0] = O[0]-Dir[0]*0.1;
      O[1] = O[1]-Dir[1]*0.1;
      O[2] = O[2]-Dir[2]*0.1;
      break;
    case 'd':
      O[0] = O[0]+left[0]*0.1;
      O[1] = O[1]+left[1]*0.1;
      O[2] = O[2]+left[2]*0.1;
      break;
    case 'a':
      O[0] = O[0]-left[0]*0.1;
      O[1] = O[1]-left[1]*0.1;
      O[2] = O[2]-left[2]*0.1;
      break;
    case 'm':
      paraS.renderMode = 1 - paraS.renderMode;
      if(paraS.renderMode == 1)
        raycaster->UpdateShadowVolume();
      printf("RenderMode: %d\n", paraS.renderMode);
      break;
    case 'v':
      raycaster->UpdateShadowVolume();
      break;
    case 'x':
      x=1-x;
      break;
    case 'p':
      if(paraS.preIntegration==0)
        paraS.preIntegration++;
      else if(paraS.preIntegration==1)
      {
        transferfunction->calculatePreIntegration2D();
        paraS.preIntegration++;
      }
      else
      {
        paraS.preIntegration = 0;
      }
      printf("preIntegration status: %d\n", paraS.preIntegration);
      break;
  }
  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
  if(showTF == 0)	 //if not adjusting the TF
  {
      if(button == GLUT_LEFT_BUTTON )
    {
      if(state == GLUT_DOWN)
        raycaster->arcBall->OnLeftButtonDown(x,y);
      else
        raycaster->arcBall->OnLeftButtonUp(x,y);
    }
    else if(button == GLUT_RIGHT_BUTTON)
    {
      if(state == GLUT_DOWN)
        raycaster->arcBall->OnRightButtonDown(x, y);
      else
        raycaster->arcBall->OnRightButtonUp(x, y);
    }
    else if(button == GLUT_MIDDLE_BUTTON)
    {
      if(state == GLUT_DOWN)
        raycaster->arcBall->OnMiddleButtonDown(x, y);
      else
        raycaster->arcBall->OnMiddleButtonUp(x, y);
    }
  }
  else
    transferfunction->MouseButton(button, state, x, raycaster->getViewportHeight()-y);
  glutPostRedisplay();

}
void move(int x, int y)
{
  transferfunction->MouseMove(x,raycaster->getViewportHeight()-y);

  raycaster->arcBall->OnMouseMove(x,y);

  //VECTOR3<float> dir(Dir[0],Dir[1],Dir[2]);
  //dir = raycaster->arcBall->GetRotation()*dir;
  //Dir[0] = dir.x;
  //Dir[1] = dir.y;
  //Dir[2] = dir.z;
  glutPostRedisplay();
}
void Save(int control)
{
  transferfunction->SavetoFile("TransparentSytle1");
}

void Apply(int control)
{
  //transferfunction->LoadfromFile(TF_string_list[curr_string]);
}

void reloadShader(int control)
{
  raycaster->ReLoadShader();
}

void parameterParser(int argc, char* argv[], rendererParameter &para)
{
  //string argument;
  vector<string> argList;
  vector<string>::iterator it;

  for(int i=1; i<argc; i++)
  {
    argList.push_back(string(argv[i]));
  }

  for(it=argList.begin(); it!=argList.end(); it++)
  {
    if( (*it).at(0) == '-')
    {
      //cout<<*it<<endl;
      if( *it== string("-dims"))
      {
        it++;
        int firstx = it->find_first_of('x');
        int lastx = it->find_last_of('x');

        para.volDimX = atoi(it->substr(0, firstx).c_str());
        para.volDimY = atoi(it->substr(firstx+1, lastx - firstx-1).c_str());
        para.volDimZ = atoi(it->substr(lastx+1, it->size() - lastx - 1).c_str());
        printf("Dim x: %d y:%d z:%d\n", para.volDimX,para.volDimY,para.volDimZ);
      }
      else if( *it == string("-viewport"))
      {
        it++;
        para.viewportX = atoi(it->c_str());
        it++;
        para.viewportY = atoi(it->c_str());
        printf("viewport: %d %d\n", para.viewportX, para.viewportY);
      }
      else if( *it == string("-filename"))
      {
        it++;
        strcpy(para.filename,(char*)it->c_str());
        printf("filename: %s\n", para.filename);
      }
      else if( *it == string("-pathfilename"))
      {
        it++;
        strcpy(para.pathFilename,(char*)it->c_str());
      }
      else if( *it == string("-type"))
      {
        it++;
        //para.fileType = (char*)it->c_str();
      }
      else if( *it == string("-help") || *it == string("--help"))
      {
        cout<<"-dims 100x100x200 -file D:/volume/head256.raw -type bin (the default type is binary)"<<endl;
      }
      else
        cout<<"error in parameter:"<<*it<<endl;
    }
    else
      cout<<"error in parameter:"<<*it<<endl;

  }

}

int main(int argc, char* argv[])
{

  rendererParameter renderPara;
  renderPara.viewportX = 800; renderPara.viewportY = 800;
  strcpy(renderPara.filename,"head256X256X256.raw");
  //renderPara.volDimX = 512;renderPara.volDimY = 512;renderPara.volDimZ = 512;
  //strcpy(renderPara.filename,"RD0019_Dark_Matter_Density_512x512x512.raw");
  //strcpy(renderPara.filename,"512-512-452-short.raw");
  //strcpy(renderPara.filename,"x_recon_torus.raw");
  //renderPara.volDimX = 512;renderPara.volDimY = 512;renderPara.volDimZ = 452;
  renderPara.volDimX = 128;renderPara.volDimY = 128;renderPara.volDimZ = 128;
  //strcpy(renderPara.filename,"h300_0063.raw");
  //renderPara.volDimX = 302; renderPara.volDimY = 302; renderPara.volDimZ = 302;

  parameterParser(argc,argv, renderPara);

  glutInit(&argc, argv);
  glutInitWindowSize(renderPara.viewportX, renderPara.viewportY);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

  window_id = glutCreateWindow("GPU RayCaster");

  //init this first
  glewInit();

  //create raycaster
  raycaster = new RayCaster;
  raycaster->Init(renderPara.viewportX, renderPara.viewportY);

  //volume3D<unsigned char> *vol = new volume3D<unsigned char>(renderPara.filename, renderPara.volDimZ ,renderPara.volDimY, renderPara.volDimX);
  //raycaster->LoadVolumeBuffer(vol->getDataBuffer(), vol->getDimZ(), vol->getDimY(), vol->getDimX(), Uchar);

  volume3D<float> *vol = new volume3D<float>(renderPara.filename, renderPara.volDimZ ,renderPara.volDimY, renderPara.volDimX);
  //volume3D<double> *vol = new volume3D<double>(renderPara.filename, renderPara.volDimZ ,renderPara.volDimY, renderPara.volDimX);
  //volume3D<short> *vol = new volume3D<short>(renderPara.filename, renderPara.volDimZ ,renderPara.volDimY, renderPara.volDimX);
  vol->updateMaxMin();
  //printf("dynamicRange: %f", vol->getDynamicRange());
  //vol->generateHistogram();
  //printf("max: %f - min: %f", vol->getMax(), vol->getMin());
  //vol->normalize();
  //vol->normalize(vol->getMin()*(119.0/256.0), vol->getMax()*(138.0/256.0));
  //vol->normalize(vol->getMin(), vol->getMax()*(2.0/256.0));
  //vol->updateMaxMin();
  //printf("max: %f - min: %f\n", float(vol->getMax()), float(vol->getMin()));
  volume3D<unsigned char>* uvol = vol->generateUcharNormalized();
  uvol->updateMaxMin();
  printf("max: %f - min: %f\n", float(uvol->getMax()), float(uvol->getMin()));
  //uvol->writeToFile("darkMatter");
  //exit(0);
  raycaster->LoadVolumeBuffer(uvol->getDataBuffer(), vol->getDimZ(), vol->getDimY(), vol->getDimX(), Uchar);
  //raycaster->LoadVolumeBuffer(vol->getDataBuffer(), vol->getDimZ(), vol->getDimY(), vol->getDimX(), Float);

  transferfunction = new TransferFunction;
  transferfunction->LoadfromFile("TransparentSytle1");
  raycaster->LoadRendererSetting("RenderStatus");
  raycaster->SetLookUpTableGLTex(transferfunction->getTexture());
  raycaster->SetCDFTableGLTex(transferfunction->getCDFTexture());
  raycaster->SetPreInt2DTableGLTex(transferfunction->getPreIntTexture2D());

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(move);

  glutMainLoop();

  return 0;
}

