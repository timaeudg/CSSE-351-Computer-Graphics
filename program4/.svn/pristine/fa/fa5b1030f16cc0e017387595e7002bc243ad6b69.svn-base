//#version 130

uniform mat4 m;

attribute vec3 pos;
attribute vec3 colorIn;
//in vec2 pos;
//in vec3 colorIn;

varying vec4 smoothColor;
//smooth out vec4 smoothColor;

void main()
{	
	vec4 p = vec4(pos, 1);
	gl_Position = m*p;
	smoothColor = vec4(colorIn, 1);
}
