
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#define GLEW_STATIC
#include "glew/glew.h"
#include <SFML/Window.hpp>
#define RESOLUTION 512

#define IMMEDIATE 1
#define VARRAY 2
#define VBO 3
#define VATTR 4
#define VAO 5

//#define RENDERMODE IMMEDIATE
//#define RENDERMODE VARRAY
//#define RENDERMODE VBO
#define RENDERMODE VATTR
//#define RENDERMODE VAO

GLuint lineBuffer;
GLuint shaderProg;
GLuint vertexAttr;
GLuint lineArray;
GLuint vertexArray;
GLint lineVertices[4] = {-1, -1, 1, 1};

void printProgramLinkLog(GLuint obj);
void printProgramCompileLog(GLuint obj);

void display()
{
	// Clear the color bits in the display buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Use a simple shader to render the line
	glUseProgram(shaderProg);
	
#if RENDERMODE == IMMEDIATE
	// Upload and render using immediate mode (~1991, 1.0)
	glBegin(GL_LINES);
	{
		glVertex2i(lineVertices[0], lineVertices[1]);
		glVertex2i(lineVertices[2], lineVertices[3]);
	}
	glEnd();
#endif
	
#if RENDERMODE == VARRAY
	// Upload and render using vertex arrays (~1997, 1.1)
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_INT, 0, lineVertices);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

#if RENDERMODE == VBO
	// Render using vertex buffers (data already on GPU) (~2003, 1.5)
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_INT, 0, 0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
	
#if RENDERMODE == VATTR
	// Render using vertex attributes (data already on GPU) (~2008, 3.0)
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, 0);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableVertexAttribArray(0);
#endif

#if RENDERMODE == VAO
	// Render using vertex array objects (data already on GPU) (~2010, 4.0)
	glBindVertexArray(vertexArray);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
#endif
	glUseProgram(0);
}

/* This function initialize the graphics state
 */
void gfxinit()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
	
	printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	
#if RENDERMODE == VATTR
	char const * vertSource = "attribute vec2 vert; void main() { gl_Position = vec4(vert.xy, 0, 1); }\0";
#else
	char const * vertSource = "void main() { gl_Position = gl_Vertex; }\0";
#endif
	char const * fragSource = "void main() { gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0); }\0";
	
	glShaderSource(vertShader, 1, (char const **)&vertSource, NULL);
	glShaderSource(fragShader, 1, (char const **)&fragSource, NULL);
	
	glCompileShader(vertShader);
	printProgramCompileLog(vertShader);
	glCompileShader(fragShader);
	printProgramCompileLog(fragShader);
	
	shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	
	glLinkProgram(shaderProg);
	printProgramLinkLog(shaderProg);
	
	vertexAttr = glGetAttribLocation(shaderProg, "vert");
	glEnableVertexAttribArray(vertexAttr);
	
#if RENDERMODE == VATTR
	glGenVertexArrays(1, &lineArray);
	glBindVertexArray(lineArray);
#endif

#if RENDERMODE == VBO || RENDERMODE == VATTR || RENDERMODE == VAO
	glGenBuffers(1, &lineBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if RENDERMODE == VAO
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, lineBuffer);
	glVertexAttribPointer(0, 2, GL_INT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glBindVertexArray(0);
#endif
}


class GLBox
{
public:
	GLBox()
	{
		// Create the main window
		App = new sf::Window(sf::VideoMode(RESOLUTION, RESOLUTION, 32), "Test Line");
		
		gfxinit();
		
		// Start render loop
		while (App->IsOpened())
		{			
			// Set the active window before using OpenGL commands
			// It's not needed here because active window is always the same,
			// but don't forget it if you use multiple windows or controls
			App->SetActive();
			
			// Handle any events that are in the queue
			sf::Event Event;
			while (App->GetEvent(Event))
			{
				// Close window : exit
				if (Event.Type == sf::Event::Closed)
					App->Close();
				
				// Escape key : exit
				if ((Event.Type == sf::Event::KeyPressed) && (Event.Key.Code == sf::Key::Escape))
					App->Close();
			}
			
			// Render the scene
			display();
			
			// Finally, display rendered frame on screen
			App->Display();
		}
	}
	
	~GLBox()
	{
		// Clean up the buffer
		glDeleteBuffers(1, &lineBuffer);
	}
private:
	sf::Window *App;
};

void printProgramCompileLog(GLuint obj)
{
	GLint infologLength;
	GLint status;
	int charsWritten = 0;
	char *infoLog;
	
	glGetProgramiv(obj, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
		return;
	
	printf("Error compiling shader: ");
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s",infoLog);
		free(infoLog);
	}
	printf("\n");
}

void printProgramLinkLog(GLuint obj)
{
	GLint infologLength;
	GLint status;
	char *infoLog;
	int charsWritten  = 0;
	
	glGetProgramiv(obj, GL_LINK_STATUS, &status);
	if (status == GL_TRUE)
		return;
	
	printf("Error linking shader: ");
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s",infoLog);
		free(infoLog);
	}
	printf("\n");
}

int main()
{
	GLBox prog;
	
    return EXIT_SUCCESS;
}
