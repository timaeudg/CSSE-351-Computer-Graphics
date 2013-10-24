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
#include "VisualStudioProj/Board.h"

#define RESOLUTION 512

#define CIRCLE_RESOLUTION 64

GLuint shaderProg;

sf::Clock clk;

/*
 Draw a single frame
 */
void display(bool pickingEnabled=false)
{
	/*
	// Clear the color bits in the display buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Render using vertex attributes (data already on GPU) (~2008, 3.0)
	// http://www.arcsynthesis.org/gltut/Basics/Tut01%20Following%20the%20Data.html
	
	// Tell OpenGL we want to use a buffer on the GPU
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	
	// Tell OpenGL what shader data slot we want to use
	glEnableVertexAttribArray(positionSlot);
	
	// Tell OpenGL how to interpret the data
	glVertexAttribPointer(positionSlot, 2, GL_FLOAT, GL_FALSE, 0, 0);

	
	// Draw some primitives as: glDrawArrays(type, base, size)
	
	// Tell OpenGL we are done with the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// Tell OpenGL we are done with the shader slot
	glDisableVertexAttribArray(positionSlot);
	glDisableVertexAttribArray(colorSlot);

	// Tell OpenGL we are done with the shader
	glUseProgram(0);
	*/
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
		board = new Board(shaderProg);
		
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
			//display();
			board->display();
		
			//printf("Frame time is %.2fs\n", clock.GetElapsedTime());
			// Finally, display rendered frame on screen
			App->Display();		

		}
	}
	
	~GLBox()
	{
		// Clean up the buffer
		delete board;
	}
private:
	sf::Window *App;
	Board* board;
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

			if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Space))
				board->newGame(true);
			
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

			if(Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::N){
				board->newGame();
			}
			//handle mouse pressed
			if(Event.Type == sf::Event::MouseButtonPressed)
			{
				if(Event.MouseButton.Button == sf::Mouse::Right) board->cancelPickup();
				if(Event.MouseButton.Button == sf::Mouse::Left) {
					int x = Event.MouseButton.X;
					int y = Event.MouseButton.Y;
					y = RESOLUTION - y;

					board->display(true);
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

					//printf("Selected: %02X %02X %02X %02X\n", (objectId >> 24) & 0xFF, (objectId >> 16) & 0xFF, (objectId >> 8) & 0xFF, objectId & 0xFF );
					if(!board->piecePlace(objectId)){
						board->piecePickUp(objectId);
					}
				}
			}
		}
	}
};

int main()
{
	printf("Program by %s+%s\n",USER1, USER2);
	GLBox prog;
	
    return EXIT_SUCCESS;
}