#ifndef __SQUARE
#define __SQUARE

#include "glew/glew.h"
#include <vector>
#include "glm/glm.hpp"
using namespace std; //makes using vectors easy

class Cube
{
public:

	Cube()
	{
		//triangle 1
#if 0
		GLfloat pos[] = {-0.1,-0.1,0, -0.1,0.1,0, 1.1,0.1,0, 1.1,-0.1,0, -0.1,-0.1,1, -0.1,0.1,1, 1.1,0.1,1, 1.1,-0.1,1};
#else
		GLfloat pos[] = {-0.1,-0.1,0, -0.1,0.1,0, 1.1,0.1,0, 1.1,-0.1,0, -0.1,-0.1,1, -0.1,0.1,1, 1.1,0.1,1, 1.1,-0.1,1};
#endif
		
		positions.insert(positions.begin(), pos, pos+3*8);
		
		for(int i=0; i<positions.size(); i++)
			positions[i] = positions[i] * 0.5;

		GLfloat col[] = {
			0.0, 0.0, 0.0,
			1.0, 0.0, 0.0,
			1.0, 1.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
			0.0, 1.0, 1.0};
		
		colors.insert(colors.begin(), col, col+3*8);
		
		GLuint ele[] = {0,1,4, 4,5,1, 0,4,3, 3,7,4, 4,5,7, 6,7,5, 1,5,2, 2,6,5, 2,3,7, 7,6,2};
		
		elements.insert(elements.begin(), ele, ele+3*12);
		
	}

	GLfloat const * getPosition() const
	{ return &positions[0]; }

	GLfloat const * getColor() const
	{ return &colors[0]; }
	
	GLuint const * getElements() const
	{ return &elements[0]; }

	size_t getVertexCount() const
	{ return positions.size()/3; }

	size_t getPositionBytes() const
	{ return positions.size()*sizeof(GLfloat); }

	size_t getColorBytes() const
	{ return colors.size()*sizeof(GLfloat); }
	
	size_t getElementBytes() const
	{ return elements.size()*sizeof(GLuint); }
	
	glm::vec3 getCentroid()
	{
		glm::vec3 center = glm::vec3(0);
		float positionCount = 1.0f/(positions.size()/3.0f);
		
		for(int i=0; i<positions.size(); i+=3)
		{
			center[0] += positions[i] * positionCount;
			center[1] += positions[i+1] * positionCount;
			center[2] += positions[i+2] * positionCount;
		}
		
		return center;
	}
	
private:
	vector<GLfloat> positions;
	vector<GLfloat> colors;
	vector<GLuint> elements;
	size_t objectCount;
};

#endif