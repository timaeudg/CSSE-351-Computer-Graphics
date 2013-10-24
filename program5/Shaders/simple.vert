//#version 130

uniform mat4 M;

attribute vec3 pos;
attribute vec3 color;
varying vec4 smoothColor;
//in vec2 pos;

void main()
{	
	smoothColor = vec4(color, 1);
	vec4 p = vec4(pos, 1);
	gl_Position = M*p;
}
