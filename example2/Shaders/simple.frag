#version 130

in vec2 texMapping;

out vec4 fragColor;

uniform sampler2D texUnit;

void main()
{
	gl_FragColor = texture(texUnit, texMapping);
	fragColor = vec4(texMapping, 1, 0);
}
