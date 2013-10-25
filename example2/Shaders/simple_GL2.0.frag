//#version 130

varying vec2 texMapping;

void main()
{
	gl_FragColor = vec4(texMapping, 1, 0);
}
