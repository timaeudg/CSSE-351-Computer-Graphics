#ifndef __OBJMESHCONVERT
#define __OBJMESHCONVERT

#include "obj_parser.h"
#include "../Mesh.h"

class OBJMeshConvert
{
public:
	Mesh loadOBJ(char * filename)
	{
		obj_scene objScene;
		parse_obj_scene(&objScene, filename);
		
		Mesh m;
		
		for(int i=0; i<objScene.vertex_count; i++)
		{
			VectorV v;
			obj_vector * oV = objScene.vertex_list[i];
			for(int c=0; c<3; c++)
				v.c[c] = oV->e[c];
			m.vertices.push_back(v);
		}
		
		for(int i=0; i<objScene.vertex_normal_count; i++)
		{
			VectorV v;
			obj_vector * oV = objScene.vertex_normal_list[i];
			for(int c=0; c<3; c++)
				v.c[c] = oV->e[c];
			m.normals.push_back(v);
		}
		
		for(int i=0; i<objScene.face_count; i++)
		{
			bool needsNormal = false;
			Triangle t;
			obj_face * oF = objScene.face_list[i];
			for(int v=0; v<3; v++)
			{
				t.vertexIndex[v] = oF->vertex_index[v];
				t.normalIndex[v] = oF->normal_index[v];
				if(t.normalIndex[v] == -1)
					needsNormal = true;
			}
			
			if(needsNormal)
			{
				VectorV v0 = m.vertices[ t.vertexIndex[0] ];
				VectorV v1 = m.vertices[ t.vertexIndex[1] ];
				VectorV v2 = m.vertices[ t.vertexIndex[2] ];
				VectorV newNorm = makeNormal(v0, v1, v2);
				m.normals.push_back(newNorm);
				t.normalIndex[0] = m.normals.size()-1;
				t.normalIndex[1] = m.normals.size()-1;
				t.normalIndex[2] = m.normals.size()-1;
			}
			
			m.triangles.push_back(t);
		}
		
		delete_obj_data(&objScene);
		
		return m;
	}
	
	VectorV cross(VectorV const & a, VectorV const & b)
	{
		VectorV dest;
		dest.c[0] = a.c[1]*b.c[2] - a.c[2]*b.c[1];
		dest.c[1] = a.c[2]*b.c[0] - a.c[0]*b.c[2];
		dest.c[2] = a.c[0]*b.c[1] - a.c[1]*b.c[0];
		return dest;
	}
	
	VectorV makeNormal(VectorV const & v0, VectorV const & v1, VectorV const & v2)
	{
		VectorV n;
		
		VectorV edge1;
		VectorV edge2;
		
		for(int i=0; i<3; i++)
		{
			edge1.c[i] = v1.c[i] - v0.c[i];
			edge2.c[i] = v2.c[i] - v1.c[i];
		}

		n = cross(edge1, edge2);
		return n;
	}
};

#endif