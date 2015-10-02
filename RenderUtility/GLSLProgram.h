#ifndef PROGRAM_H
#define PROGRAM_H

#define GLEW_STATIC

#include <GL/glew.h>
#include <iostream>

#define MAXPROGRAM 10

class GLSLProgram
{
public:
  GLSLProgram();
  ~GLSLProgram();
  bool MakeProgram(const char* vertexsh, const char* geometrysh=0, const char* fragmentsh=0);
  bool MakeProgramFromString(std::string* vertString, std::string* fragString);
  void BeginProgram(); //glUseProgram(prog)
  void EndProgram();
  void BindTexture(unsigned int unit,GLenum, GLuint, const char* nameinshader);

  void setUniform1f(const char*,float);
  void setUniform3f(const char* nameinshader, float* value);
  void setUniformMatf(const char*, float*);
  void setUniformMat3f(const char*, float*);
  void setUniform1uint(const char*, unsigned int);
  void setUniform3uint(const char* nameinshader, unsigned int x,unsigned int y, unsigned int z);
  static std::string* LoadShaderToString(const char* filename);


private:
  GLuint prog;
  void _loadshader(const char* shadername, char* buffer);
  void _printlog(GLuint obj);
  static void checkInput(const char * filename);

  //handle
  char* buffer;
  GLint len;


};



#endif
