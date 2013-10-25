#ifndef __RENDERENGINE
#define __RENDERENGINE

#include <SFML/Graphics.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLHelper.h"
#include "Model.h"

#define zOffset 600.0f

class RenderEngine
{
public:
	RenderEngine()
	{
		initialized = false;
		this->P = glm::ortho(-1, 1, -1, 1);
	}

	~RenderEngine()
	{
		if(initialized)
		{
			// Clean up the buffers
			glDeleteBuffers(1, &positionBuffer);
			glDeleteBuffers(1, &texCoordBuffer);
			glDeleteVertexArrays(2, vertexArray);
		}
	}

	void init(unsigned int const & w, unsigned int const & h)
	{
		this->w = w;
		this->h = h;
		clock.Reset();
		
		setupGlew();
		setupTextures();
		setupShader();
		
		model = Model();
		e = model.getCenter() + glm::vec3(0,0,zOffset);
		
		this->P = glm::perspective(90.0f, 1.0f, 0.1f, 5000.0f);
		
		setupBuffers();
		
		initialized = true;
	}

	void display()
	{
		//glEnable(GL_DEPTH_TEST);

		//clear the old frame
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//use shader
		glm::vec3 c = model.getCenter();
		glm::vec3 u = glm::vec3(0,1,0);
		glm::mat4 C = glm::lookAt(e, c, u);;
		
		glUseProgram(textureProg);
		glUniformMatrix4fv(glGetUniformLocation(textureProg, "C"), 1, GL_FALSE, &C[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(textureProg, "P"), 1, GL_FALSE, &P[0][0]);
		glBindVertexArray(vertexArray[0]);

		int texUnitId = 0;
		glUniform1i(glGetUniformLocation(textureProg, "texUnit"), texUnitId);
		
		int numQuads = model.getVertexCount()/6;
		for(int i=0; i<numQuads; i++)
		{
			//bind the sampler to a texture unit here
			glActiveTexture(GL_TEXTURE0 + texUnitId);
			glBindTexture(GL_TEXTURE_2D, textures[i]);

			glDrawArrays(GL_TRIANGLES, 6*i, 6);
		}
		
		bool drawWireframe = true;
		if(drawWireframe) {
			glUseProgram(wireframeProg);
			glBindVertexArray(vertexArray[1]);
			glUniformMatrix4fv(glGetUniformLocation(textureProg, "C"), 1, GL_FALSE, &C[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(textureProg, "P"), 1, GL_FALSE, &P[0][0]);
			int numTris = model.getVertexCount()/3;
			for(int i=0; i<numQuads; i++)
			{
				glDrawArrays(GL_LINE_LOOP, 3*i, 3);
			}
		}
		
		//cleanup
		glBindVertexArray(0);
		glUseProgram(0);
		checkGLError("display");
	}
	
	void reshape(int const & newWidth, int const & newHeight)
	{
		glViewport(0, 0, newWidth, newHeight);
	}
	
	void moveCamera(glm::vec2 const & motionVector)
	{
		this->e = this->e + glm::vec3(motionVector, 0);
		this->e = glm::clamp(this->e, model.getMinBound(), model.getMaxBound());
		this->e.z = zOffset;
	}

private:
	Model model;
	bool initialized;
	sf::Clock clock;

	GLuint textureProg;
	GLuint wireframeProg;

	GLuint positionBuffer;
	GLuint texCoordBuffer;
	GLuint vertexArray[2];
	GLuint textures[4];
	
	unsigned int w;
	unsigned int h;
	
	glm::mat4 P;
	glm::vec3 e;

	void setupGlew()
	{
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			exit(1);
		}
		printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}

	void setupShader()
	{
		char const * vertPath = "Shaders/simple.vert";
		char const * fragPath = "Shaders/simple.frag";
		char const * vert20Path = "Shaders/simple_GL2.0.vert";
		char const * frag20Path = "Shaders/simple_GL2.0.frag";
		char const * wireFragPath = "Shaders/wireframe.frag";

		textureProg = ShaderManager::shaderFromFile(&vertPath, &fragPath, 1, 1);
		
		//for macs in compatibility mode
		//textureProg = ShaderManager::shaderFromFile(&vert20Path, &frag20Path, 1, 1);
		
		wireframeProg = ShaderManager::shaderFromFile(&vert20Path, &wireFragPath, 1, 1);

		checkGLError("shader");
	}

	void setupTextures()
	{
		//generate texture names
		sf::Image image;
		glGenTextures(4, textures);
		char * imagePaths[4] = {"Images/rhit1.png", "Images/rhit2.png", "Images/rhit3.png", "Images/rhit4.png"};
		
		for(int i=0; i<4; i++)
		{
			image.LoadFromFile(imagePaths[i]);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			//control sampling here
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//control boundaries here
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			//upload and generate mipmap
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixelsPtr());
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		checkGLError("texture");
	}

	void setupBuffers()
	{
		//setup position buffer
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), model.getPositions(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// now the texture coords
		glGenBuffers(1, &texCoordBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getTexCoordBytes(), model.getTexCoords(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//setup the vertex array
		glGenVertexArrays(2, vertexArray);
		
		//first one for texture rendering
		{
		GLint positionSlot = glGetAttribLocation(textureProg, "pos");
		GLint texCoordSlot = glGetAttribLocation(textureProg, "texCoord");
		glBindVertexArray(vertexArray[0]);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glEnableVertexAttribArray(texCoordSlot);
		glVertexAttribPointer(texCoordSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindVertexArray(0);
		}
		
		//second one for wireframe rendering
		{
		GLint positionSlot = glGetAttribLocation(wireframeProg, "pos");
		GLint texCoordSlot = glGetAttribLocation(wireframeProg, "texCoord");
		glBindVertexArray(vertexArray[1]);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glEnableVertexAttribArray(texCoordSlot);
		glVertexAttribPointer(texCoordSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glBindVertexArray(0);
		}
		
		checkGLError("setup");
	}
};

#endif