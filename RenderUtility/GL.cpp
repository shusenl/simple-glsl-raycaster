#include "GL.h"
#include <stdio.h>

GL::GL() {}

GL::~GL() {}

void GL::InitGLContext()
{

}

void GL::InitGLStatus(glInitStatus &GLstatus)
{
  //pixel store
  glPixelStorei(GL_UNPACK_ALIGNMENT, GLstatus.pixelUnpackedAlignment);
  glPixelStorei(GL_PACK_ALIGNMENT, GLstatus.pixelPackedAlignment);

}

void GL::SetupPerspectView(int w, int h, float *rotMat4X4, float *transVec3, float *eye )
{
  //set up views
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, w/h, 0.0, 25.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  //rotate
  //glTranslatef( 0.0, 0.1, 3.0);
  glTranslatef( transVec3[0], transVec3[1], transVec3[2] );
  glMultMatrixf(rotMat4X4);

}
void GL::SetupOrthogonalView(int viewport_ox, int viewport_oy,int viewport_w, int viewport_h,
                        int left, int right, int bottom, int top)
{
  if (viewport_h == 0)
    viewport_h = 1;
  glViewport(viewport_ox, viewport_oy, viewport_w, viewport_h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(left, right, bottom, top);//xmin, xmax, ymin, ymax

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void GL::DrawRenderTarget(int o_x, int o_y, int x, int y, GLuint tex)
{
  //glMatrixMode(GL_MODELVIEW);
  //glPushMatrix();
  //glMatrixMode(GL_PROJECTION);
  //glPushMatrix();

  glBindTexture(GL_TEXTURE_2D,tex);
  glEnable(GL_TEXTURE_2D);

  //origin,x,y, w, h
  SetupOrthogonalView(o_x,o_y, x, y);
  glColor4f(1.0f,1.0f,1.0f,1.0f);
  DrawFullScreenQuad();

  glDisable(GL_TEXTURE_2D);

  //glMatrixMode(GL_MODELVIEW);
  //glPopMatrix();
  //glMatrixMode(GL_PROJECTION);
  //glPopMatrix();

}
//==============draw geometry
//drawing geometry
void GL::DrawFullScreenQuad()
{
  //glDisable(GL_DEPTH_TEST);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0);
  glVertex2f(0,0);
  glTexCoord2f(1,0);
  glVertex2f(1,0);
  glTexCoord2f(1, 1);
  glVertex2f(1, 1);
  glTexCoord2f(0, 1);
  glVertex2f(0, 1);

  glEnd();
  //glEnable(GL_DEPTH_TEST);
}

void GL::DrawFullScreenQuadNegativeOneToOne()
{
  glColor3f(0,0,0);
  glBegin(GL_QUADS);
  glVertex2f(-1.0, 1.0);
  glVertex2f(1.0, 1.0);
  glVertex2f(1.0, -1.0);
  glVertex2f(-1.0, -1.0);
  glEnd();
}

//=============================================================
//matrix
void GL::BuildViewingMatrix(GLfloat* modelViewMatrix, GLfloat translate[], GLfloat invRotMat[])
{
  //memset(invRotMat,0,sizeof(GLfloat)*16);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(invRotMat);
  glTranslatef( -translate[0], -translate[1], -translate[2]+5.0f );
  //printf("%f  %f  %f \n", m_pos[0], m_pos[1], m_pos[2]);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
  glPopMatrix();
}
void GL::BuildViewingMatrixNoTranslation(GLfloat* modelViewMatrix, GLfloat invRotMat[])
{
  //memset(invRotMat,0,sizeof(GLfloat)*16);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixf(invRotMat);
  //glTranslatef( -translate[0], -translate[1], -translate[2]+5.0f );
  //printf("%f  %f  %f \n", m_pos[0], m_pos[1], m_pos[2]);
  glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
  glPopMatrix();
}




void GL::CheckErrors(void)
{
  //if (!s_current)
  //	return;

  GLenum err = glGetError();
  const char* name;
  switch (err)
  {
    case GL_NO_ERROR:                       name = NULL; break;
    case GL_INVALID_ENUM:                   name = "GL_INVALID_ENUM"; break;
    case GL_INVALID_VALUE:                  name = "GL_INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:              name = "GL_INVALID_OPERATION"; break;
    case GL_STACK_OVERFLOW:                 name = "GL_STACK_OVERFLOW"; break;
    case GL_STACK_UNDERFLOW:                name = "GL_STACK_UNDERFLOW"; break;
    case GL_OUT_OF_MEMORY:                  name = "GL_OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:  name = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT"; break;
    default:                                name = "unknown"; break;
  }

  if (name)
    printf("Caught GL error 0x%04x (%s)!", err, name);
}
