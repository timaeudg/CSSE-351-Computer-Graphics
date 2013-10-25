
#include "GLHelper.h"
#include <stdio.h>

void glWindowPos4fMESAemulate(GLfloat x,GLfloat y,GLfloat z,GLfloat w)
{
	GLfloat fx, fy;

	/* Push current matrix mode and viewport attributes. */
	glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

	/* Setup projection parameters. */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDepthRange(z, z);
	glViewport((int) x - 1, (int) y - 1, 2, 2);

	/* Set the raster (window) position. */
	fx = x - (int) x;
	fy = y - (int) y;
	glRasterPos4f(fx, fy, 0.0, w);

	/* Restore matrices, viewport and matrix mode. */
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

void glWindowPos2fMESAemulate(GLfloat x, GLfloat y)
{
	glWindowPos4fMESAemulate(x, y, 0, 1);
}


void moveWindowPos2f(GLfloat x, GLfloat y)
{
	glBitmap(0,0, 0,0, x, y, NULL);
}

void glutBitmapStringEmulate(void * font, char const * string)
{
	char *c;
	for (c= (char*) string; *c != '\0'; c++)
	{
		//glutBitmapCharacter(font, *c);
	}
}

void print3dBitmapString(void * font, char const * string, float x, float y, float z, bool drawShadow)
{
	float currentColor[4];

	if(drawShadow)
	{
		glGetFloatv(GL_CURRENT_COLOR, currentColor);

		glColor4f(0.0, 0.0, 0.0, 0.5);
		glRasterPos3f( x, y, z );
		moveWindowPos2f(1.0, -1.0);
		glutBitmapStringEmulate(font, string);

		glColor4fv(currentColor);
	}
	glRasterPos3f( x, y, z );
	glutBitmapStringEmulate(font, string);
}

void printOutlineBitmapString(void * font, char const * string)
{
	float currentColor[4];
	float currentRasterPos[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);
	glGetFloatv(GL_CURRENT_RASTER_POSITION, currentRasterPos);

	glColor4f(0.0, 0.0, 0.0, 1.0);
	glWindowPos4fMESAemulate(currentRasterPos[0], currentRasterPos[1], currentRasterPos[2], currentRasterPos[3]);
	moveWindowPos2f(1.0, 0.0);
	glutBitmapStringEmulate(font, string);
	glWindowPos4fMESAemulate(currentRasterPos[0], currentRasterPos[1], currentRasterPos[2], currentRasterPos[3]);
	moveWindowPos2f(-1.0, 0.0);
	glutBitmapStringEmulate(font, string);
	glWindowPos4fMESAemulate(currentRasterPos[0], currentRasterPos[1], currentRasterPos[2], currentRasterPos[3]);
	moveWindowPos2f(0.0, 1.0);
	glutBitmapStringEmulate(font, string);
	glWindowPos4fMESAemulate(currentRasterPos[0], currentRasterPos[1], currentRasterPos[2], currentRasterPos[3]);
	moveWindowPos2f(0.0, -1.0);
	glutBitmapStringEmulate(font, string);

	glColor4fv(currentColor);  //reset to original color
	glWindowPos4fMESAemulate(currentRasterPos[0], currentRasterPos[1], currentRasterPos[2], currentRasterPos[3]);
	glutBitmapStringEmulate(font, string);
}
void checkGLError()
{
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
	   fprintf (stderr, "OpenGL Error: %s\n", errString);
	}
}
