#ifndef __TEXTUREBUFFEROBJECT
#define __TECTUREBUFFEROBJECT

#include <vector>
#include <assert.h>
#include <SFML/Window.hpp>
#include "GLHelper.h"

#define DEFAULT_FBO 0

class TextureBufferObject
{
public:
	
	TextureBufferObject()
	{
		this->numBuffers = 0;
		this->resolution = 0;
	}
	
	TextureBufferObject(int const numBuffers, int const resolution)
	{
		this->numBuffers = numBuffers;
		this->resolution = resolution;
		allocBuffers(numBuffers);
	}
	
	int newBufferSet(int const startIndex, int const setSize)
	{
		assert(startIndex + setSize <= numBuffers);
		
		int setId = bufferSetId.size();
		
		bufferSetId.push_back(setId);
		bufferSetSize.push_back(setSize);
		bufferSetStartId.push_back(startIndex);
		
		return setId;
	}
	
	void activateBufferSet(int const setIndex)
	{
		int enumIndex = bufferSetStartId[setIndex];
		GLuint * startPos = &colorBuffers[enumIndex];
		int setSize = bufferSetSize[setIndex];
		glDrawBuffers(setSize, startPos);
	}
	
	void activateTextureSet(int const setIndex, int const shaderProg)
	{
		char shaderCharId = '0';
		char shaderTexName[] = "texInId0";
		int startIndex = bufferSetStartId[setIndex];
		int setSize = bufferSetSize[setIndex];
		
		for(int i=0; i<setSize; i++)
		{
			int texName = getTexture( startIndex + i );
			shaderTexName[7] = '0' + i;
			glUniform1i(glGetUniformLocation(shaderProg, shaderTexName),i);
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_RECTANGLE, texName);
#ifdef visualizeMaps
			//printf("%s:%i->%i\n", shaderTexName, i, texName);
#endif
		}
#ifdef visualizeMaps
		//printf("\n");
#endif
	}
	
	void activateTexture(int const texIndex, int const shaderProg)
	{
		char shaderCharId = '0';
		char shaderTexName[] = "texInId0";
		int setSize = getTexture(texIndex);
		
		shaderTexName[7] = '0' + texIndex;
		glUniform1i(glGetUniformLocation(shaderProg, shaderTexName),texIndex);
		glActiveTexture(GL_TEXTURE0+texIndex);
		glBindTexture(GL_TEXTURE_RECTANGLE, getTexture(texIndex));
	}
	
	void activateAllBuffers()
	{ glDrawBuffers(numBuffers, colorBuffers); }
	
	void activateSingleBuffer(int const bufferIndex)
	{ glDrawBuffers(1, &colorBuffers[bufferIndex]); }
	
	void bindFrameBuffer()
	{ glBindFramebuffer(GL_FRAMEBUFFER, fboId); }
	
	void bindDefaultFrameBuffer()
	{ glBindFramebuffer(GL_FRAMEBUFFER, DEFAULT_FBO); }
	
	GLuint getTexture(int const objIndex)
	{ return textureObjects[objIndex]; }
	
	GLenum getBuffer(int const objIndex)
	{ return colorBuffers[objIndex]; }
	GLenum getBuffer(int const setIndex, int const objIndex)
	{
		int enumIndex = bufferSetStartId[setIndex];
		GLuint * startPos = &colorBuffers[enumIndex];
		GLuint bufferName = startPos[objIndex];
		return bufferName;
	}
	
	GLuint getTexture(int const setIndex, int const objIndex)
	{
		int startIndex = bufferSetStartId[setIndex];
		GLuint * startPos = &textureObjects[startIndex];
		GLuint textureName = startPos[objIndex];
		return textureName;
	}
	
	GLuint getBufferName()
	{ return fboId; }
	
	
private:
	int numBuffers;
	int resolution;
	GLuint fboId;
	GLuint depthBuffer;
	GLenum * colorBuffers;
	GLuint * textureObjects;
	
	std::vector<int> bufferSetId;
	std::vector<int> bufferSetSize;
	std::vector<int> bufferSetStartId;
	
	void allocBuffers(int const numBuffers)
	{
		int maxInternal = 10; //due to shader variable name assignment
		int maxBuffers = 0;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxBuffers);
		
		int maxTotal = maxBuffers;
		if(maxTotal > maxInternal)
			maxTotal = maxInternal;
		
		if(numBuffers > maxTotal)
		{
			printf("Too many GL buffers, max is %i.\n", maxTotal);
			exit(2);
		}
		
		colorBuffers = (GLenum*) malloc( sizeof(GLenum)*numBuffers );
		textureObjects = (GLuint*) malloc( sizeof(GLuint)*numBuffers );
		
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		
		// Create the render buffer for depth
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, resolution, resolution);
		
		// Attach the depth render buffer to the FBO as it's depth attachment
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		
		//setup render targets
		printf("Alloc textures[%i]: ", numBuffers);
		glGenTextures(numBuffers, textureObjects);
		for (int i=0; i<numBuffers; i++)
		{
			printf("%i ", textureObjects[i]);
			setupTexture(textureObjects[i]);
		}
		printf("\n");
		
		//Bind output textures
		printf("Bind texture to color buffer[%i]: ", numBuffers);
		for (int i=0; i<numBuffers; i++)
		{
			printf("%i=%i ", i, textureObjects[i]);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, textureObjects[i], 0);
		}
		printf("\n");
		
		//attach targets to framebuffer
		for (int i=0; i<numBuffers; i++)
			colorBuffers[i] = GL_COLOR_ATTACHMENT0+i;
		
		glDrawBuffers(numBuffers, colorBuffers);
		
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status != GL_FRAMEBUFFER_COMPLETE)
		{
			fprintf(stderr, "Frame buffer setup failed!\n");
			exit(1);
		}
		
		this->bindDefaultFrameBuffer();
		checkGLError();
		
	}
	
	void deallocBuffers()
	{
	}
	void setupTexture(int id)
	{
		glBindTexture(GL_TEXTURE_RECTANGLE,id);
		//see if it's faster with a different format: GL_RGBA16F, GL_RGBA8
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA32F,  resolution, resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
};

#endif