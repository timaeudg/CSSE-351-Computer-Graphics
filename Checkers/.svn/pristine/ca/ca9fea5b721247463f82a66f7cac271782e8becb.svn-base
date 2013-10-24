#version 150

in vec2 pos;

uniform mat3 transform;

void main()
{
	gl_Position = vec4(transform * vec3(pos, 1), 1);
}