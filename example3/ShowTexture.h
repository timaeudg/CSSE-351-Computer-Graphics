#ifndef __SHOWTEXTURE
#define __SHOWTEXTURE

#include <SFML/Window.hpp>

#if defined(_WIN32)

#elif defined(__linux__) || defined(__linux)

#elif defined(__APPLE__)
#include <GLUT/glut.h>
#endif

#include "GLHelper.h"

class ShowTexture
{
public:
	bool GL20Support;
	ShowTexture()
	{
		showNumber = false;
		clearBuffers = false;
		drawBuffers = false;
		sleepTime = 0;
		alreadyUsingProgram = false;
		this->drawTexShader = 0;
		GL20Support = false;
	}
	
	ShowTexture(GLuint const drawTexShader)
	{
		showNumber = false;
		clearBuffers = false;
		drawBuffers = false;
		sleepTime = 0;
		alreadyUsingProgram = false;
		this->drawTexShader = drawTexShader;
		GL20Support = false;
	}
	
	void setSleepTime(int const sleepTime)
	{ this->sleepTime = sleepTime; }
	void setClearBuffers(bool const clearBuffers)
	{ this->clearBuffers = clearBuffers; }
	void setDrawBuffers(bool const drawBuffers)
	{ this->drawBuffers = drawBuffers; }
	void setShowTextureNumber(bool const showNumber)
	{ this->showNumber = showNumber; }
	
	void setTextureNumber(int const numToShow=0)
	{ this->numToShow = numToShow; }
	
	void render(GLuint textureName, int texSize[2], int viewSize[2])
	{
		if(clearBuffers)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawTexture(textureName, texSize);
		if(showNumber)
			drawTextureNumber(numToShow, texSize);
		if(drawBuffers)
		{
			//glutSwapBuffers();
			//glutPostRedisplay();
		}
		//if(sleepTime > 0)
		//	Sleep(sleepTime);
	}
	
	void useProgram()
	{
		if(GL20Support)
			glUseProgram(drawTexShader);
		else
			glUseProgramObjectARB(drawTexShader);
		alreadyUsingProgram = true;
	}
	
	GLuint getProgram()
	{
		return drawTexShader;
	}
	
private:
	bool showNumber;
	bool clearBuffers;
	bool drawBuffers;
	bool alreadyUsingProgram;
	int sleepTime;
	int numToShow;
	GLuint drawTexShader;
	
	void drawTexture(int textureId, int textureSize[2])
	{
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glEnable (GL_TEXTURE_2D);
		
		if(alreadyUsingProgram)
			alreadyUsingProgram = false;
		else
			useProgram();
		
		//use shader to display texture
		int textureUnit = 0;
		if(GL20Support)
		{
			glUniform1i(glGetUniformLocation(drawTexShader, "texId"),textureUnit);
			glUniform2f(glGetUniformLocation(drawTexShader, "resolution"), textureSize[0], textureSize[1]);
			glActiveTexture(GL_TEXTURE0+textureUnit);
		}
		else
		{
			glUniform1iARB(glGetUniformLocationARB(drawTexShader, "texId"),textureUnit);
			glUniform2fARB(glGetUniformLocationARB(drawTexShader, "resolution"), textureSize[0], textureSize[1]);
			glActiveTexture(GL_TEXTURE0+textureUnit);
		}
		glBindTexture(GL_TEXTURE_2D, textureId);
		
		drawRenderTexture(textureSize[0], textureSize[1], textureSize[0], textureSize[1]);
		
		if(GL20Support)
			glUseProgram(0);
		else
			glUseProgramObjectARB(0);
	}
	
	void drawTextureNumber(int numToShow, int texSize[2])
	{
		/*
		if(numToShow>9)
			return;
		
		char tStr[] = "0\0";
		tStr[0] += numToShow;
		//printf("%s\n", tStr);
		
		void *font = GLUT_BITMAP_HELVETICA_18;
		int halfGlyph = glutBitmapWidth(font, tStr[0]) / 2.0;
		
		glColor4f(1,1,1,1);
		glWindowPos2fMESAemulate(texSize[0]/2 - halfGlyph, texSize[1]/2);
		printOutlineBitmapString(font, tStr);
		//glutBitmapStringEmulate(font, tStr);
		*/
	}
	
	void drawRenderTexture(int const viewportX, int const viewportY, int const textureX, int const textureY)
	{
		glViewport(0, 0, viewportX, viewportY);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,1,0,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		GLfloat projMatrix[16];
		GLfloat viewMatrix[16];
		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
		
		if(GL20Support)
		{
			glUniformMatrix4fv(glGetUniformLocation(drawTexShader, "projMatrix"), 1, false, projMatrix);
			glUniformMatrix4fv(glGetUniformLocation(drawTexShader, "viewMatrix"), 1, false, viewMatrix);
		}
		else
		{
			glUniformMatrix4fvARB(glGetUniformLocationARB(drawTexShader, "projMatrix"), 1, false, projMatrix);
			glUniformMatrix4fvARB(glGetUniformLocationARB(drawTexShader, "viewMatrix"), 1, false, viewMatrix);
		}
		
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2f(0,0);
		glTexCoord2i(1, 0); glVertex2f(1,0);
		glTexCoord2i(1, 1); glVertex2f(1,1);
		glTexCoord2i(0 ,1); glVertex2f(0,1);
		glEnd();
	}
};


#endif
