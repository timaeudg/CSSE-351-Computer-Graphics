#version 130

uniform float timeIn;
in vec4 smoothColor;
out vec4 fragColor;

void main()
{
	float cyclicTime = 0.5*sin(timeIn)+0.5;
	fragColor = vec4(smoothColor.rg, cyclicTime, smoothColor.a);
}