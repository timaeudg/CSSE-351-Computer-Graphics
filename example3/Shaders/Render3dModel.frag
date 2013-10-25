#version 120

varying vec3 fColor;
varying float d;
varying float s;

void main()
{
	vec4 blueColor = vec4(0.1, 0.6, 0.8, 1.0);
	vec4 white = vec4(1);
	
	vec4 aColor = blueColor * 0.02; //same color as diffuse, just darker
	vec4 dColor = d * blueColor;
	vec4 sColor = s * white;
	gl_FragData[0] = aColor + dColor + sColor;
}