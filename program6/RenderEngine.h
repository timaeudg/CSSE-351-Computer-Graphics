#ifndef __RENDERENGINE
#define __RENDERENGINE

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "GLHelper.h"
#include "MazeModel.h"
#include "Wall.h"
using namespace glm;
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
			glDeleteBuffers(1, &elementBuffer);
			glDeleteVertexArrays(1, &vertexArray);
		}
	}

	void init(unsigned int const & w, unsigned int const & h)
	{
		this->w = w;
		this->h = h;
		clock.Reset();
		setupGlew();
		setupShader();
		generateMaze();
		initialized = true;
	}

	void display(vec3 camera = vec3(1,3,5), vec3 target = vec3(3,3,0), bool outside = true)
	{
		glEnable(GL_DEPTH_TEST);

		//clear the old frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//use shader
		glUseProgram(shaderProg);
		
		//draw
		glBindVertexArray(vertexArray);

		mat4 C = mat4(1);
		if(outside){ 
			C = glm::lookAt(vec3((model.width+1)/2, 
							     (model.height+1)/2, 
								 ((model.width > model.height) ? 
									(float)model.width/2. : 
			                        (float)model.height/2.)+2), 
							vec3((model.width+1)/2, (model.height+1)/2, 0),
							vec3(0,0.001,1));

			P = perspective(90.f, 1.f, ((model.width > model.height) ? 
											(float)model.width/2.f : 
											(float)model.height/2.f)-2, 
									   ((model.width > model.height) ? 
											(float)model.width/2.f : 
											((float)model.height/2.f))+3);
		} else {
			C = glm::lookAt(camera, target, vec3(0,0.001,1));
			P = perspective(90.f, 1.f, 0.1f, 100.f);
		}

		glUniformMatrix4fv(cameraSlot, 1, GL_FALSE, &C[0][0]);
		glUniformMatrix4fv(perspectiveSlot, 1, GL_FALSE, &P[0][0]);
		GLfloat t = clock.GetElapsedTime();
		glUniform1f(timeSlot, t);

		for(int k = 0; k < model.getElementCount(); k+=2){
			GLuint first = model.getElements()[k];
			GLuint second = model.getElements()[k+1];
			float xPos = first % (model.width+1);
			float yPos = int(first/(model.width+1));
			//printf("Pos = %f, %f\n", xPos, yPos);
			fflush(stdout);
			glm::mat4 T = glm::translate(glm::mat4(1), glm::vec3(xPos,yPos,0));
			if(second - first > 1){
				//Vertical Down
				T = glm::rotate(T, 90.f, glm::vec3(0,0,1));
			}
			glUniformMatrix4fv(matSlot, 1, GL_FALSE, &T[0][0]);
			glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
			//glDrawElements(GL_LINES, model.getElementCount(), GL_UNSIGNED_INT, 0);
		
		}
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30*sizeof(GLuint)));
		/*glm::mat4 T = glm::scale(glm::mat4(1), glm::vec3(200,200,1));
		T = P * T;
		glUniformMatrix4fv(matSlot, 1, GL_FALSE, &T[0][0]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/
		

		//cleanup
		glBindVertexArray(0);
		glUseProgram(0);
		checkGLError("display");
	}
	
	void reshape(int const & newWidth, int const & newHeight)
	{
		glViewport(0, 0, newWidth, newHeight);
	}
	
	void generateMaze(unsigned int const & seed = 1)
	{
		Maze mazeLayout(w, h, seed);
		model = MazeModel(mazeLayout);
		maze = mazeLayout;
		
		this->P = glm::ortho(-model.getUnitSize(), (w+1)*model.getUnitSize(), -model.getUnitSize(), (h+1)*model.getUnitSize());
	
		if(initialized)
			rebuildBuffers();
		else
			setupBuffers();
	}
	
	Maze maze;

private:
	MazeModel model;
	bool initialized;

	GLuint shaderProg;

	GLuint positionBuffer;
	GLuint elementBuffer;
	GLuint colorBuffer;
	GLuint normBuffer;
	GLuint vertexArray;

	GLint positionSlot;
	GLint matSlot;
	GLint perspectiveSlot;
	GLint cameraSlot;
	GLint colorSlot;
	GLint normSlot;
	GLint timeSlot;

	sf::Clock clock;
	
	unsigned int w;
	unsigned int h;
	
	glm::mat4 P;

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

		shaderProg = ShaderManager::shaderFromFile(&vertPath, &fragPath, 1, 1);

		// Find out where the shader expects the data
		positionSlot = glGetAttribLocation(shaderProg, "pos");
		matSlot =      glGetUniformLocation(shaderProg, "M");
		perspectiveSlot = glGetUniformLocation(shaderProg, "P");
		cameraSlot = glGetUniformLocation(shaderProg, "C");
		colorSlot = glGetAttribLocation(shaderProg, "color");
		normSlot = glGetAttribLocation(shaderProg, "norm");
		timeSlot = glGetUniformLocation(shaderProg, "time");

		checkGLError("shader");
	}

	void setupBuffers()
	{

		//setup the vertex array
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		//setup position buffer
		glGenBuffers(1, &positionBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Wall::points), Wall::points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//setup normal Buffer
		printf("Norm Slot: %d\n", normSlot);
		glGenBuffers(1, &normBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Wall::normals), Wall::normals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(normSlot);
		glVertexAttribPointer(normSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);



		// Do the same thing for the color data
		printf("Color Slot: %d\n", colorSlot);
		glGenBuffers(1, &colorBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Wall::colors), Wall::colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(colorSlot);
		glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// now the elements
		glGenBuffers(1, &elementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Wall::elements), Wall::elements, GL_STATIC_DRAW);

		glBindVertexArray(0);
		
		checkGLError("setup");
	}
	
	void rebuildBuffers()
	{
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Wall::points), Wall::points);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Wall::elements), Wall::elements);

		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Wall::colors), Wall::colors, GL_STATIC_DRAW);
		
		
		glBindVertexArray(vertexArray);	
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glEnableVertexAttribArray(positionSlot);
		glVertexAttribPointer(positionSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
		glBindVertexArray(0);
		checkGLError("rebuild");
	}
};

#endif