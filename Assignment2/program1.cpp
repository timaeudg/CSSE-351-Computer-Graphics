//This is program1 modified to use ShaderManager

#define USER1 "Eric Guilford"
#define USER2 "Devon Timaeus"

#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <cmath>

#define GLEW_STATIC
#include "glew/glew.h"
#include <SFML/Window.hpp>
#include "ShaderManager.h"

#define RESOLUTION 512

#define CIRCLE_RESOLUTION 64
#define CIRCLE_RADIUS 0.4

#define LOOP_POINTS 4
#define LINE_POINTS 4
#define POINT_POINTS 2
#define TRIANGLE_POINTS 3

const int TOTAL_POINTS = CIRCLE_RESOLUTION + LOOP_POINTS + LINE_POINTS + POINT_POINTS + TRIANGLE_POINTS;

GLuint shaderProg;

GLuint positionBuffer;
GLuint colorBuffer;
GLuint identityBuffer;

GLuint positionSlot;
GLuint colorSlot;
GLuint timeSlot;
GLuint pickingSlot;
GLuint identitySlot;

GLuint ids[TOTAL_POINTS] = {200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
							200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
							200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
							200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200,
							300, 300, 300, 300, 350, 350, 400, 400, 450, 450, 500, 500, 500};

GLfloat* positions;
GLfloat colors[3*TOTAL_POINTS] = {1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1, 1,1,1,
								  1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0, 1,0,0,
								  0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0, 0,1,0,
								  0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1, 0,0,1,
								  0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5, 0.5,0.5,0.5,
							      0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1, 0,1,1,
								  1,1,0, 1,1,0, 1,1,0, 1,1,0, 1,0,1, 1,0,1, 1,0,1, 1,0,1, 0.5,0.5,0.5, 0.5,0.5,0.5,
								  0.5,0.5,0.5, 0.5,0.5,0.5, 1,1,0, 1,1,0, 0,0,0, 0,0,0, 0,0,0};

sf::Clock clk;

/*
 Draw a single frame
 */
void display(bool pickingEnabled=false)
{
	// Clear the color bits in the display buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Use a simple shader to render the line
	glUseProgram(shaderProg);

	GLfloat currentTime = clk.GetElapsedTime();
	glUniform1f(timeSlot, currentTime);

	if(pickingEnabled) {
		glUniform1i(pickingSlot, GL_TRUE);
	}
	else{
		glUniform1i(pickingSlot, GL_FALSE);
	}
	
	// Render using vertex attributes (data already on GPU) (~2008, 3.0)
	// http://www.arcsynthesis.org/gltut/Basics/Tut01%20Following%20the%20Data.html
	
	// Tell OpenGL we want to use a buffer on the GPU
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	
	// Tell OpenGL what shader data slot we want to use
	glEnableVertexAttribArray(positionSlot);
	
	// Tell OpenGL how to interpret the data
	glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	// Do the same thing for colors
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glEnableVertexAttribArray(colorSlot);
	glVertexAttribPointer(colorSlot, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//identity attribute
	glBindBuffer(GL_ARRAY_BUFFER, identityBuffer);
	glEnableVertexAttribArray(identitySlot);
	glVertexAttribIPointer(identitySlot, 1, GL_UNSIGNED_INT, 0, 0);

	
	// Draw some primitives as: glDrawArrays(type, base, size)
	glDrawArrays(GL_TRIANGLE_FAN, 0, CIRCLE_RESOLUTION);
	glDrawArrays(GL_LINE_LOOP, CIRCLE_RESOLUTION, LOOP_POINTS);
	glDrawArrays(GL_LINES, CIRCLE_RESOLUTION + LOOP_POINTS, LINE_POINTS);
	glDrawArrays(GL_POINTS, CIRCLE_RESOLUTION + LOOP_POINTS + LINE_POINTS, POINT_POINTS);
	glDrawArrays(GL_TRIANGLES, CIRCLE_RESOLUTION + LOOP_POINTS + LINE_POINTS + POINT_POINTS, TRIANGLE_POINTS);
	
	// Tell OpenGL we are done with the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Tell OpenGL we are done with the shader slot
	glDisableVertexAttribArray(positionSlot);
	glDisableVertexAttribArray(colorSlot);
	glDisableVertexAttribArray(identitySlot);
	
	// Tell OpenGL we are done with the shader
	glUseProgram(0);
}

void generatePointsForDrawing() {
	//malloc space for the circle points
	GLfloat *generatedPositions = (GLfloat *) malloc(2 * sizeof(GLfloat) * TOTAL_POINTS );
	GLfloat pointX = 0;
	GLfloat pointY = 0;
	
	*generatedPositions = pointX;
	*(generatedPositions + 1) = pointY;

	int outerPoints = CIRCLE_RESOLUTION-2;

	double anglePerPoint = 2 * M_PI / outerPoints;
	
	for(int k = 0; k <= outerPoints ; k++) {
		pointX = CIRCLE_RADIUS * cos(anglePerPoint * k);
		pointY = CIRCLE_RADIUS * sin(anglePerPoint * k);
		*(generatedPositions + 2 + 2*k) = pointX;
		*(generatedPositions + 3 + 2*k) = pointY;
	}
	
	int currentPosition = 2*CIRCLE_RESOLUTION;
	//square code
	*(generatedPositions + currentPosition) = 0.5;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.5;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.5;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.5;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.5;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.5;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.5;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.5;
	++currentPosition;
	//line code
	*(generatedPositions + currentPosition) = -0.8;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.7;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.8;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.7;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.8;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.7;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.8;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.7;
	++currentPosition;
	//POINT points
	*(generatedPositions + currentPosition) = 0.6;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.6;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0;
	++currentPosition;
	//TRIANGLE points
	*(generatedPositions + currentPosition) = 0.0;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.3;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.2;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.2;
	++currentPosition;
	*(generatedPositions + currentPosition) = -0.2;
	++currentPosition;
	*(generatedPositions + currentPosition) = 0.2;
	++currentPosition;

	positions = generatedPositions;
}


/*
 Initialize the graphics state
 */
void graphicsInit()
{
	// glew will help us use GL functions, so set it up here
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
	
	//Setup shaders
	char const * vertPath = "Shaders/simple.vert";
	char const * fragPath = "Shaders/simple.frag";
	shaderProg = ShaderManager::shaderFromFile(&vertPath, &fragPath, 1, 1);
	
	// The data we will render needs to be on the GPU
	// These commands upload the data
	
	// Find out where the shader expects the data
	positionSlot = glGetAttribLocation(shaderProg, "pos");
	colorSlot = glGetAttribLocation(shaderProg, "color");
	identitySlot = glGetAttribLocation(shaderProg, "identity");
	timeSlot = glGetUniformLocation(shaderProg, "time");
	pickingSlot = glGetUniformLocation(shaderProg, "pickingEnabled");

	// Generate a GPU side buffer
	glGenBuffers(1, &positionBuffer);
	
	// Tell OpenGL we want to work with the buffer we just made
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	generatePointsForDrawing();
	
	// Allocate and upload data to GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 *TOTAL_POINTS, positions, GL_STATIC_DRAW);
	
	// Do the same thing for the color data
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

	glGenBuffers(1, &identityBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, identityBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ids), ids, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

class GLBox
{
public:
	GLBox()
	{
		// Create the main window
		App = new sf::Window(sf::VideoMode(RESOLUTION, RESOLUTION, 32), "program1");
		
		sf::Clock clock;
		clock.Reset();
		graphicsInit();
		
		
		// Start render loop
		while (App->IsOpened())
		{			
			// Set the active window before using OpenGL commands
			// It's not needed here because the active window is always the same,
			// but don't forget it if you use multiple windows or controls
			App->SetActive();
		
			// Handle any events that are in the queue
			handleEvents();
		
			// Render the scene
			display();
		
			//printf("Frame time is %.2fs\n", clock.GetElapsedTime());
			// Finally, display rendered frame on screen
			App->Display();		

		}
	}
	
	~GLBox()
	{
		// Clean up the buffer
		glDeleteBuffers(1, &positionBuffer);
	}
private:
	sf::Window *App;
	void handleEvents()
	{
		sf::Event Event;
		while (App->GetEvent(Event))
		{
			// Close window : exit
			if (Event.Type == sf::Event::Closed)
				App->Close();
			
			// Escape key : exit
			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
				App->Close();
			
			// This is for grading your code. DO NOT REMOVE
			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Equal)
			{
				unsigned char *dest;
				unsigned int w = App->GetWidth();
				unsigned int h = App->GetHeight();
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				dest = (unsigned char *) malloc( sizeof(unsigned char)*w*h*3);
				glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, dest);
				
				FILE *file;
				file = fopen("_program1.ppm", "wb");
				fprintf(file, "P6\n%i %i\n%i\n", w, h, 255);
				for(int i=h-1; i>=0; i--)
					fwrite(&dest[i*w*3], sizeof(unsigned char), w*3, file);
				fclose(file);
				free(dest);
			}
			
			//handle mouse pressed
			if(Event.Type == sf::Event::MouseButtonPressed)
			{
				if(Event.MouseButton.Button == sf::Mouse::Left) {
					int x = Event.MouseButton.X;
					int y = Event.MouseButton.Y;
					y = RESOLUTION - y;

					display(true);
					glFlush();
					glFinish();
				
					float clickColor[4];
					glPixelStorei(GL_PACK_ALIGNMENT, 1);
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, clickColor);

					GLuint clickId = 0;
					clickId = clickId | ((unsigned int)(clickColor[0] * 255.0f) << 0);
					clickId = clickId | ((unsigned int)(clickColor[1] * 255.0f) << 8);
					clickId = clickId | ((unsigned int)(clickColor[2] * 255.0f) << 16);
					clickId = clickId | ((unsigned int)(clickColor[3] * 255.0f) << 24);

					unsigned int objectId = clickId;
					switch(objectId) {
						case 200: printf("You clicked the rainbow circle!\n"); break;
						case 500: printf("That triangles really moving!\n"); break;
						case 350: printf("Sweet! You clicked the one of the lines!\n"); break;
						case 300: printf("You clicked the loop/square!\n"); break;
						case 450: printf("Wow, you managed to click one of the points!!!\n"); break;
					};
				}
			}
		}
	}
};

int main()
{
	printf("Program by %s+%s\n", "USER1", "USER2");
	GLBox prog;
	
    return EXIT_SUCCESS;
}