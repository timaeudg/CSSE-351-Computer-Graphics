#ifndef __MESH_
#define __MESH_

#include <vector>

class VectorV
{
public:
	float c[3];
};

class Triangle
{
public:
	int vertexIndex[3];
	int normalIndex[3];;
};

class Mesh
{
public:
	std::vector<VectorV> vertices;
	std::vector<VectorV> normals;
	std::vector<Triangle> triangles;
	
	VectorV getCenter()
	{
		VectorV min = getMinBound();
		VectorV max = getMaxBound();
		VectorV center;
		
		for(int c=0; c<3; c++)
			center.c[c] = (min.c[c]+max.c[c]) * 0.5;
		
		return center;
	}
	
	VectorV getMinBound()
	{
		VectorV bound;

		for(int c=0; c<3; c++)
		{
			bound.c[c] = 9999999.0;
		}
		
		for(int i=0; i<this->vertices.size(); i++)
		{
			for(int c=0; c<3; c++)
			{
				if(vertices[i].c[c] < bound.c[c])
					bound.c[c] = vertices[i].c[c];
			}
		}
		
		return bound;
	}
	
	VectorV getMaxBound()
	{
		VectorV bound;
		
		for(int c=0; c<3; c++)
		{
			bound.c[c] = -9999999.0;
		}
		
		for(int i=0; i<this->vertices.size(); i++)
		{
			for(int c=0; c<3; c++)
			{
				if(vertices[i].c[c] > bound.c[c])
					bound.c[c] = vertices[i].c[c];
			}
		}
		
		return bound;
	}
};

#endif