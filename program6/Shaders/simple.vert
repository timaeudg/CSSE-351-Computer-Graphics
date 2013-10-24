//#version 130

uniform mat4 M;
uniform mat4 P;
uniform mat4 C;
uniform float time;

in vec3 pos;
in vec3 color;
in vec3 norm;

varying vec4 smoothColor;
//in vec2 pos;

void main()
{	
	
	smoothColor = vec4(color, 1);
	smoothColor = M*vec4(norm, 0);
	smoothColor = smoothColor + vec4(0,0,0,1);
	vec4 p = vec4(pos, 1);
	gl_Position = P*C*M*p;
}
