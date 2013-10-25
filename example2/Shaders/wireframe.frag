//#version 130

uniform sampler2D texSampler;
varying vec2 texMapping;

void main()
{
	gl_FragColor = vec4(texMapping, 1, 1);
}
