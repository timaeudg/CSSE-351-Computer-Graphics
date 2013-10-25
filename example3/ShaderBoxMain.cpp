
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "glew/glew.h"
#include <SFML/Window.hpp>
#include <math.h>

#include "Config.h"
#include "Teapot.h"
#include "ShaderManager.h"
#include "TextureBufferObject.h"
#include "ShowTexture.h"
#include "Mesh.h"
#include "obj_loader/OBJMeshConvert.h"

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <unistd.h>
#include <string.h>
#endif

class ShaderBox
{
public:
	ShaderBox()
	{
		// Create the main window
		App = new sf::Window(sf::VideoMode(RES, RES, 32), "Shader Box");
		
		// Create a clock for measuring time elapsed
		Clock = sf::Clock();
		
		__glewInit();
		
		currentRes[0] = RES;
		currentRes[1] = RES;
		
		//setup render target texture
		//this will eventually hald the rendered scene and be
		//rendered to a quad for post process effects
		int numTex = 1;
		glGenTextures(numTex, &textureTarget);
		setupTargetTexture();
		
		//setup the shader programs
		//the first set is the vertex and fragment shaders for the 3D render
		//the second set is the shaders for the texture post process effects
		ShaderManager shaders;
		shaders.GL20Support = GL20Support;
		char const * drawVert = "Shaders/Render3dModel.vert";
		char const * drawFrag = "Shaders/Render3dModel.frag";
		shaderProg = shaders.buildShaderProgram(&drawVert, &drawFrag, 1, 1);
		
		char const * texVert = "Shaders/RenderTexture.vert";
		char const * texFrag = "Shaders/RenderTexture.frag";
		texProg = shaders.buildShaderProgram(&texVert, &texFrag, 1, 1);
		
		//this object helps draw textures that fill the viewport
		texRender = ShowTexture(texProg);
		texRender.GL20Support = GL20Support;
		
		//load mesh
		FILE *file = NULL;
		file = fopen("model.obj", "rb");
		
		if(file != NULL)
		{
			fclose(file);
			OBJMeshConvert objConvert;
			mesh = objConvert.loadOBJ("model.obj");
		}
		else
		{
			mesh = buildTeapotMesh();
		}
		VectorV cent = mesh.getCenter();
		for(int i=0; i<3; i++)
			center[i] = cent.c[i];
		VectorV min = mesh.getMinBound();
		VectorV max = mesh.getMaxBound();
		VectorV size = max;
		for(int i=0; i<3; i++)
			size.c[i] -= min.c[i];
		biggestSize = -99999.0;
		for(int i=0; i<3; i++)
			if( size.c[i] > biggestSize)
				biggestSize = size.c[i];
		
		printf("bounds (%f %f %f) .. ", min.c[0], min.c[1], min.c[2]);
		printf("(%f %f %f)\n", max.c[0], max.c[1], max.c[2]);
		
		_near = biggestSize*0.2;
		_far = biggestSize*2;
		fov = 90.f;
		cameraPos[0] = center[0];
		cameraPos[1] = center[1];
		cameraPos[2] = center[2]+biggestSize;
		lookAtPos[0] = center[0];
		lookAtPos[1] = center[1];
		lookAtPos[2] = center[2];
		cameraUp[0] = 0;
		cameraUp[1] = 1;
		cameraUp[2] = 0;
		
		lightPos[0] = center[0]+biggestSize;
		lightPos[1] = center[1]+biggestSize/2.0f;
		lightPos[2] = center[2]+biggestSize;
		
		printf("eye %f %f %f\n", cameraPos[0], cameraPos[1], cameraPos[2]);
		printf("center %f %f %f\n", lookAtPos[0], lookAtPos[1], lookAtPos[2]);
		printf("up %f %f %f\n", cameraUp[0], cameraUp[1], cameraUp[2]);
		printf("light %f %f %f\n", lightPos[0], lightPos[1], lightPos[2]);
		
		//init to starting event state
		mouseButtonDown = false;
		cursorScrollAmount = 0.0;
		cursorDragAmount[0] = 0.0;
		cursorDragAmount[1] = 0.0;
		
		// Start render loop
		while (App->IsOpened())
		{
			// Process events
			handleEvents();
			
			// Set the active window before using OpenGL commands
			// It's useless here because active window is always the same,
			// but don't forget it if you use multiple windows or controls
			App->SetActive();
			
			//render 3D scene, then put 2D output in texture, render texture
			renderScene();
						
			// Finally, display rendered frame on screen
			App->Display();
		}
	}
	
	~ShaderBox()
	{
	}
private:
	GLuint textureTarget;
	GLuint shaderProg;
	GLuint texProg;
	ShowTexture texRender;
	sf::Window *App;
	sf::Clock Clock;
	Mesh mesh;
	float _near;
	float _far;
	float fov;
	float cursorScrollAmount;
	float center[3];
	float cameraPos[3];
	float lookAtPos[3];
	float cameraUp[3];
	float lightPos[3];
	float lastClickPos[2];
	float lastFrameDragPos[2];
	float cursorDragAmount[2];
	float cursorAbsolutePos[2];
	int currentRes[2];
	bool mouseButtonDown;
	float biggestSize;
	
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
			
			if(Event.Type == sf::Event::MouseButtonPressed)
			{
				lastClickPos[0] = Event.MouseButton.X;
				lastClickPos[1] = (currentRes[1]-Event.MouseButton.Y);
				lastFrameDragPos[0] = Event.MouseButton.X;
				lastFrameDragPos[1] = (currentRes[1]-Event.MouseButton.Y);
				mouseButtonDown = true;
			}
			
			if(Event.Type == sf::Event::MouseButtonReleased)
				mouseButtonDown = false;
			
			if(Event.Type == sf::Event::MouseMoved && mouseButtonDown)
			{				
				cursorDragAmount[0] += lastFrameDragPos[0] - Event.MouseMove.X;
				cursorDragAmount[1] += lastFrameDragPos[1] - (currentRes[1]-Event.MouseMove.Y);
				lastFrameDragPos[0] = Event.MouseMove.X;
				lastFrameDragPos[1] = (currentRes[1]-Event.MouseMove.Y);
			}
			
			if(Event.Type == sf::Event::MouseWheelMoved)
			{				
				cursorScrollAmount += Event.MouseWheel.Delta;
			}
			
			if(Event.Type == sf::Event::MouseMoved)
			{
				cursorAbsolutePos[0] = Event.MouseMove.X;
				cursorAbsolutePos[1] = (currentRes[1]-Event.MouseMove.Y);
			}
			
			// Resize event : adjust viewport
			if (Event.Type == sf::Event::Resized)
			{
				glViewport(0, 0, Event.Size.Width, Event.Size.Height);
				currentRes[0] = Event.Size.Width;
				currentRes[1] = Event.Size.Height;
				setupTargetTexture();
			}
		}
	}
	
	void renderScene()
	{
		// Set color and depth clear value
		glClearDepth(1.f);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		
		// Enable Z-buffer read and write
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		
		// Setup a perspective projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float ratio = ((float)currentRes[0]/(float)currentRes[1]);
		gluPerspective(fov, ratio, _near, _far);
		
		// Clear color and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Apply some transformations
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(cameraPos[0], cameraPos[1], cameraPos[2],
				  lookAtPos[0], lookAtPos[1], lookAtPos[2],
				  cameraUp[0], cameraUp[1], cameraUp[2]);
		
		lightPos[0] = center[0]+cos(Clock.GetElapsedTime())*biggestSize;
		lightPos[1] = center[1]+biggestSize/2.0f;
		lightPos[2] = center[2]+sin(Clock.GetElapsedTime())*biggestSize;
		
		//variables for 3D render shader
		if(GL20Support)
			glUseProgram(shaderProg);
		else
			glUseProgramObjectARB(shaderProg);
		setShaderVariables(shaderProg);

		// Draw the model
		glBegin(GL_TRIANGLES);
		
		for(int t=0; t<mesh.triangles.size(); t++)
		{
			Triangle &tri = mesh.triangles[t];
			
			for(int v=0; v<3; v++)
			{
				int vertexIndex = tri.vertexIndex[v];
				int normalIndex = tri.normalIndex[v];
				VectorV vertex = mesh.vertices[vertexIndex];
				if(normalIndex > -1)
				{
					VectorV normal = mesh.normals[normalIndex];
					glNormal3fv( normal.c );
				}
				glVertex3fv( vertex.c );
			}
		}
		
		glEnd();
		
		//copy buffer to texture
		glBindTexture(GL_TEXTURE_2D,textureTarget);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, currentRes[0], currentRes[1], 0);
		texRender.useProgram();
		
		//variables for texture shader
		//texture unit id is handled in ShowTexture
		setShaderVariables(texRender.getProgram());
		
		//sets up a few shader variables and draws the texture on a full view quad
		texRender.render(textureTarget, currentRes, currentRes);
	}
	
	void setShaderVariables(GLuint shaderProg)
	{
		GLfloat projMatrix[16];
		GLfloat viewMatrix[16];
		glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX, viewMatrix);
		
		if(GL20Support)
		{
			glUniform1f(glGetUniformLocation(shaderProg, "elapsedTime"), Clock.GetElapsedTime());
			glUniform1f(glGetUniformLocation(shaderProg, "near"), _near);
			glUniform1f(glGetUniformLocation(shaderProg, "far"), _far);
			glUniform1f(glGetUniformLocation(shaderProg, "fov"), fov);
			glUniform1f(glGetUniformLocation(shaderProg, "cursorScrollAmount"), cursorScrollAmount);
			glUniform2f(glGetUniformLocation(shaderProg, "resolution"), currentRes[0], currentRes[1]);
			glUniform3f(glGetUniformLocation(shaderProg, "modelCenter"),  center[0], center[1], center[2]);
			glUniform3f(glGetUniformLocation(shaderProg, "cameraPos"),  cameraPos[0], cameraPos[1], cameraPos[2]);
			glUniform3f(glGetUniformLocation(shaderProg, "lookAtPos"),  lookAtPos[0], lookAtPos[1], lookAtPos[2]);
			glUniform3f(glGetUniformLocation(shaderProg, "cameraUp"),  cameraUp[0], cameraUp[1], cameraUp[2]);
			glUniform3f(glGetUniformLocation(shaderProg, "lightPos"),  lightPos[0], lightPos[1], lightPos[2]);
			glUniformMatrix4fv(glGetUniformLocation(shaderProg, "projMatrix"), 1, false, projMatrix);
			glUniformMatrix4fv(glGetUniformLocation(shaderProg, "viewMatrix"), 1, false, viewMatrix);
			glUniform2f(glGetUniformLocation(shaderProg, "cursorAbsolutePos"), cursorAbsolutePos[0], cursorAbsolutePos[1]);
			glUniform2f(glGetUniformLocation(shaderProg, "cursorDragAmount"), cursorDragAmount[0], cursorDragAmount[1]);
			glUniform2f(glGetUniformLocation(shaderProg, "lastClickPos"), lastClickPos[0], lastClickPos[1]);
			glUniform2f(glGetUniformLocation(shaderProg, "lastFrameDragPos"), lastFrameDragPos[0], lastFrameDragPos[1]);
			glUniform1i(glGetUniformLocation(shaderProg, "mouseButtonDown"), mouseButtonDown);
		}
		else
		{
			glUniform1fARB(glGetUniformLocationARB(shaderProg, "elapsedTime"), Clock.GetElapsedTime());
			glUniform1fARB(glGetUniformLocationARB(shaderProg, "near"), _near);
			glUniform1fARB(glGetUniformLocationARB(shaderProg, "far"), _far);
			glUniform1fARB(glGetUniformLocationARB(shaderProg, "fov"), fov);
			glUniform1fARB(glGetUniformLocationARB(shaderProg, "cursorScrollAmount"), cursorScrollAmount);
			glUniform2fARB(glGetUniformLocationARB(shaderProg, "resolution"), currentRes[0], currentRes[1]);
			glUniform3fARB(glGetUniformLocationARB(shaderProg, "modelCenter"),  center[0], center[1], center[2]);
			glUniform3fARB(glGetUniformLocationARB(shaderProg, "cameraPos"),  cameraPos[0], cameraPos[1], cameraPos[2]);
			glUniform3fARB(glGetUniformLocationARB(shaderProg, "lookAtPos"),  lookAtPos[0], lookAtPos[1], lookAtPos[2]);
			glUniform3fARB(glGetUniformLocationARB(shaderProg, "cameraUp"),  cameraUp[0], cameraUp[1], cameraUp[2]);
			glUniform3fARB(glGetUniformLocationARB(shaderProg, "lightPos"),  lightPos[0], lightPos[1], lightPos[2]);
			glUniformMatrix4fvARB(glGetUniformLocationARB(shaderProg, "projMatrix"), 1, false, projMatrix);
			glUniformMatrix4fvARB(glGetUniformLocationARB(shaderProg, "viewMatrix"), 1, false, viewMatrix);
			glUniform2fARB(glGetUniformLocationARB(shaderProg, "cursorAbsolutePos"), cursorAbsolutePos[0], cursorAbsolutePos[1]);
			glUniform2fARB(glGetUniformLocationARB(shaderProg, "cursorDragAmount"), cursorDragAmount[0], cursorDragAmount[1]);
			glUniform2fARB(glGetUniformLocationARB(shaderProg, "lastClickPos"), lastClickPos[0], lastClickPos[1]);
			glUniform2fARB(glGetUniformLocationARB(shaderProg, "lastFrameDragPos"), lastFrameDragPos[0], lastFrameDragPos[1]);
			glUniform1iARB(glGetUniformLocationARB(shaderProg, "mouseButtonDown"), mouseButtonDown);
		}
	}
	
	void setupTargetTexture()
	{
		glBindTexture(GL_TEXTURE_2D,textureTarget);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  currentRes[0], currentRes[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	
	void __glewInit()
	{
		GL20Support = false;
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			fprintf(logFile, "Error: %s\n", glewGetErrorString(err));
		}
		else
		{
			printf("GLEW init finished...\n");
			fprintf(logFile, "GLEW init finished...\n");
			if( __GLEW_VERSION_2_0 )
			{
				printf("OpenGL 2.0 is supported. Shaders should run correctly.\n");
				fprintf(logFile, "OpenGL 2.0 is supported. Shaders should run correctly.\n");
				GL20Support = true;
			}
			else
			{
				printf("OpenGL 2.0 is NOT enabled. The program may not work correctly.\n");
				fprintf(logFile, "OpenGL 2.0 is NOT enabled. The program may not work correctly.\n");
			}
			
			if( GLEW_ARB_vertex_program )
			{
				printf("ARB vertex programs supported.\n");
				fprintf(logFile, "ARB vertex programs supported.\n");
			}
			else
			{
				printf("ARB vertex programs NOT supported. The program may not work correctly.\n");
				fprintf(logFile, "ARB vertex programs NOT supported. The program may not work correctly.\n");
			}
			if( GLEW_ARB_fragment_program )
			{
				printf("ARB fragment programs supported.\n");
				fprintf(logFile, "ARB fragment programs supported.\n");
			}
			else
			{
				printf("ARB fragment programs NOT supported. The program may not work correctly.\n");
				fprintf(logFile, "ARB fragment programs NOT supported. The program may not work correctly.\n");
			}
		}
	}
};

FILE * logFile;
bool GL20Support;


int main()
{
#ifdef __APPLE__
#define pathSize 5000
	char path[pathSize];
	uint32_t size = pathSize;
    _NSGetExecutablePath(path, &size);
	char *slashPos = strrchr(path, '/');
	slashPos[0] = '\0';
	chdir(path);
	chdir("../../../");
#endif
	
	GL20Support = false;
	logFile = fopen("log.txt", "w");
	ShaderBox prog;
	fclose(logFile);
	
    return EXIT_SUCCESS;
}
