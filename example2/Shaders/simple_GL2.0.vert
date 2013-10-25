//#version 130

uniform mat4 P;
uniform mat4 C;

attribute vec2 pos;
attribute vec2 texCoord;

varying vec2 texMapping;

void main()
{	
	vec4 p = vec4(pos, 0, 1);
	
	texMapping = texCoord;
	gl_Position = P*C*p;
}
