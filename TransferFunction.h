#ifndef TRANSFORFUNCTION_H
#define TRANSFORFUNCTION_H
//#include<windows.h>
#include<GL/glew.h>
#include <iostream>
#include <fstream>
#include <list>
#include <iterator>

#define TABLESIZE 256

#define FACTOR 4
#define MARGIN 2
#define COLORPIKER_Y 50*FACTOR
#define COLORPIKER_X 30*FACTOR
#define HISTO_X 150*FACTOR
#define HISTO_Y 50*FACTOR
#define COLORSHOW_Y 5*FACTOR

#define WIDTH MARGIN + COLORPIKER_X + HISTO_X + MARGIN
#define HEIGHT MARGIN + COLORPIKER_Y + MARGIN

#define LEFT_BUTTON 0
#define MIDDLE_BUTTON 1
#define RIGHT_BUTTON 2

#define BIAS 10

struct Key
{
  int ID; //0 .. 255
  int posx, posy;
  float color[4]; //RGBA
  bool active;

  friend std::ostream& operator<<(std::ostream&, const Key&);
  friend std::istream& operator>>(std::istream&, Key&);
};

class TransferFunction
{
public:
  TransferFunction();
    ~TransferFunction();
  void Render();
  bool MouseButton(int button, int state, int x, int y);//state 0 down 1 up
  bool MouseMove(int x, int y);
  void SavetoFile(const char*) ;
  void LoadfromFile(const char *);
  void setHistogram(double* his) {histogram = his; }
  GLuint getTexture() {return Lut;}
  GLuint getCDFTexture(){return PLut;}
  GLuint getPreIntTexture2D(){return PLut2D;}
  GLuint getPreIntTexture1D(){return PLut1D;}
  //pre-integration
  void calculatePreIntegration();
  void calculatePreIntegration2D();
private:
  void _addkey(std::list<Key>::iterator it,Key key);
  void _deletekey(std::list<Key>::iterator it);
  void _setkeycolor(std::list<Key>::iterator it, float color[]);
  void _updatecolortable();
  bool _notinrang(int x, int y);
  int _classify(int posx, int posy);
  void _generatecolorpalette();

  unsigned char colortable[TABLESIZE][4];
  float CDFtable[TABLESIZE][4];
  float preIntegrateTable[TABLESIZE][TABLESIZE][4];
  float preIntegrateTable1D[TABLESIZE][4];

  float **colorpalette; //256*3
  double *histogram;
  GLuint Lut, PLut, PLut1D, PLut2D;
  std::list<Key> Keylist;

  //states
  int offsetx, offsety;
    //pointer to the key being selected or the key closest to the mouse hit point
  std::list<Key>::iterator currentkey;
  int preIntegrationMode;

  //mouse
  int mousedownpos[2];
  bool dragenable;
};


#endif
