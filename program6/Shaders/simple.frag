//#version 130
varying vec4 smoothColor;
varying vec4 surfaceNormal;
varying vec4 lightVector;
varying vec4 viewVector;

void main()
{
	vec4 aColor = 0.15*smoothColor;
	vec4 dColor = clamp(dot(lightVector, surfaceNormal), 0.0, 1.0)*smoothColor;
	vec4 sColor = pow(clamp(dot(viewVector,reflect(-lightVector,surfaceNormal)), 0.0, 1.0),10.0)*vec4(1,1,1,1);
	gl_FragColor = dColor+aColor+sColor;
}
