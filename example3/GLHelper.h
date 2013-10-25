#ifndef __GLHELPER_
#define __GLHELPER_

#include <SFML/Window.hpp>

#if defined(_WIN32)

#elif defined(__linux__) || defined(__linux)

#elif defined(__APPLE__)
#include <GLUT/glut.h>
#endif

void glWindowPos4fMESAemulate(GLfloat x,GLfloat y,GLfloat z,GLfloat w);
void glWindowPos2fMESAemulate(GLfloat x, GLfloat y);
void moveWindowPos2f(GLfloat x, GLfloat y);
void glutBitmapStringEmulate(void * font, char const * string);
void print3dBitmapString(void * font, char const * string, float x, float y, float z, bool drawShadow=false);
void printOutlineBitmapString(void * font, char const * string);
void checkGLError();

#endif