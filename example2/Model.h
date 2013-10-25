#ifndef __MODEL
#define __MODEL

#include <vector>

using namespace std; //makes using vectors easy

class Model
{
public:
	
	Model()
	{
		//leftmost  quad, uses 1st image
		positions.push_back(0);
		positions.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(1);
		positions.push_back(422);
		positions.push_back(0);
		texCoords.push_back(1);
		texCoords.push_back(1);
		positions.push_back(422);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
		positions.push_back(0);
		positions.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(422);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(0);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
		//middle quad, uses 2nd image
		positions.push_back(422);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(818);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(818);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
		positions.push_back(422);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(818);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(422);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
		//bottom quad, uses 3rd image
		positions.push_back(422);
		positions.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(1156);
		positions.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(1156);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
		positions.push_back(422);
		positions.push_back(0);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(1156);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(422);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
		//right most upper quad, uses 4th image
		positions.push_back(818);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(1156);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(1156);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
		positions.push_back(818);
		positions.push_back(56);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(1156);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		positions.push_back(818);
		positions.push_back(187);
		texCoords.push_back(0);
		texCoords.push_back(0);
		
	}
	
	GLfloat const * getPositions() const
	{ return &positions[0]; }
	
	size_t getPositionBytes() const
	{ return positions.size()*sizeof(GLfloat); }

	GLfloat const * getTexCoords() const
	{ return &texCoords[0]; }
	
	size_t getTexCoordBytes() const
	{ return texCoords.size()*sizeof(GLfloat); }
	
	size_t getVertexCount() const
	{ return positions.size(); }
	
	glm::vec3 getCenter()
	{
		glm::vec3 min = getMinBound();
		glm::vec3 max = getMaxBound();
		glm::vec3 center;
		
		for(int c=0; c<2; c++)
			center = (min+max) * 0.5f;
		
		return center;
	}
	
	glm::vec3 getMinBound()
	{
		glm::vec3 bound;
		bound = glm::vec3(9999999.0);
		
		for(unsigned int i=0; i<this->positions.size()/2; i++)
		{
			for(int c=0; c<2; c++)
			{
				if(positions[i*2+c] < bound[c])
					bound[c] = positions[i*2+c];
			}
		}
		
		return bound;
	}
	
	glm::vec3 getMaxBound()
	{
		glm::vec3 bound;
		bound = glm::vec3(-9999999.0);
		
		for(unsigned int i=0; i<this->positions.size()/2; i++)
		{
			for(int c=0; c<2; c++)
			{
				if(positions[i*2+c] > bound[c])
					bound[c] = (positions[i*2+c]);
			}
		}
		
		return bound;
	}
	
private:
	vector<GLfloat> positions;
	vector<GLfloat> texCoords;
};

#endif
