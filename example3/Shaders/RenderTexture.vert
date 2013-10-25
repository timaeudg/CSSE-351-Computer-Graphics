// You do not need to edit this shader

uniform mat4 projMatrix;  //projection matrix
uniform mat4 viewMatrix;  //view matrix (camera)

void main()
{
	gl_Position = projMatrix * viewMatrix * gl_Vertex;
}
