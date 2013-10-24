#ifndef __RENDERENGINE
#define __RENDERENGINE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLHelper.h"
#include "Cube.h"

class RenderEngine
{
public:
	RenderEngine()
	{
		initialized = false;
		this->P = glm::mat4(1);
		
		//camera
		glm::vec3 e, c, u;
		e = glm::vec3(0,0,3);
		c = glm::vec3(0,0,2);
		u = glm::vec3(0,1,0);
		this->C = glm::lookAt(e, c, u);
		
		this->M = glm::mat4(1);
	}

	~RenderEngine()
	{
		if(initialized)
		{
			// Clean up the buffers
			glDeleteBuffers(1, &positionBuffer);
			glDeleteBuffers(1, &colorBuffer);
		}
	}

	void init()
	{
		initialized = true;
		model = Cube();
		clk.Reset();
		
		GLint vp[4];
		glGetIntegerv(GL_VIEWPORT, vp);

		setupGlew();
		setupShader();
		setupBuffers();
	}

	void display(bool pickingEnabled=false)
	{
		glEnable(GL_DEPTH_TEST);

		//clear the old frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//use shader
		glUseProgram(shaderProg);
		glm::mat4 T = P*C*M;
		glUniformMatrix4fv(matSlot, 1, GL_FALSE, &T[0][0]);
		
		//draw
		glBindVertexArray(vertexArray);
		glDrawElements(GL_TRIANGLES, 3*12, GL_UNSIGNED_INT, 0);
		
		//cleanup
		glBindVertexArray(0);
		glUseProgram(0);
	}
	
	void setProjectionTransform(glm::mat4 const & transform)
	{
		this->P = transform;
	}
	
	void setModelTransform(glm::mat4 const & transform)
	{
		this->M = transform;
	}
	
	void reshape(int const & newWidth, int const & newHeight)
	{
		glViewport(0, 0, newWidth, newHeight);
	}

private:
	Cube model;
	bool initialized;

	GLuint colorfulProg;
	GLuint shaderProg;

	GLuint positionBuffer;
	GLuint colorBuffer;
	GLuint elementBuffer;
	GLuint vertexArray;

	GLint positionSlot;
	GLint colorSlot;
	GLint timeSlot;
	GLint matSlot;
	sf::Clock clk;
	
	glm::mat4 P;
	glm::mat4 C;
	glm::mat4 M;

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
		char const * redPath = "Shaders/simple.frag";

		shaderProg = ShaderManager::shaderFromFile(&vertPath, &redPath, 1, 1);

		// Find out where the shader expects the data
		positionSlot = glGetAttribLocation(shaderProg, "pos");
		colorSlot =    glGetAttribLocation(shaderProg, "colorIn");
		timeSlot =    glGetUniformLocation(shaderProg, "timeIn");
		matSlot =     glGetUniformLocation(shaderProg, "m");

		checkGLError("shader");
	}

	void setupBuffers()
	{
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		
		//setup position buffer
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getPositionBytes(), model.getPosition(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Do the same thing for the color data
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, model.getColorBytes(), model.getColor(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(colorSlot);
		glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		// now the elements
		glGenBuffers(1, &elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getElementBytes(), model.getElements(), GL_STATIC_DRAW);
		//leave the element buffer active
		
		glBindVertexArray(0);

		checkGLError("setup");
	}
};

#endif