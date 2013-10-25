#version 120

// All model vertices and normals are passed to this shader.
// It must transform the vertices as appropriate for the view.
// The view and projection matrices are provided, if you need
// the normal matrix, you must construct it in the shader.

uniform float elapsedTime;  //time in seconds since simulation began
uniform float near; //near srendering plane
uniform float far; //far rendering plane
uniform float fov; //field of view
uniform float cursorScrollAmount; //delta scroll amount
uniform vec2 resolution;  //view resolution in pixels
uniform vec3 modelCenter;  //center of the model (might be useful for rotations)
uniform vec3 cameraPos;  //camera position
uniform vec3 lookAtPos;  //point in 3D space the camera is point towards
uniform vec3 cameraUp;  //camera up vector
uniform mat4 projMatrix;  //projection matrix
uniform mat4 viewMatrix;  //view matrix (camera)
uniform vec2 cursorAbsolutePos;  //absolute cursor position in pixels
uniform vec2 cursorDragAmount;  //tracks amount of x,y change while the mouse button is down (i.e. amount of drag)
uniform vec2 lastClickPos;  //where the last mouse click happened
uniform vec2 lastFrameDragPos;  //where the cursor was on the previous frame while the mouse button is down
uniform int mouseButtonDown;  //tracks if the mouse button is down

varying vec3 fColor;
varying float d;
varying float s;

vec4 lightPos = vec4(0,4,4,1);
mat4 rotateY(float a)
{
	return mat4(
		cos(a), 0, -sin(a), 0,
		0, 1, 0, 0,
		sin(a), 0, cos(a), 0,
		0, 0, 0, 1);
}

void main()
{
	mat4 P = projMatrix;
	mat4 C = viewMatrix;
	mat4 R = rotateY(elapsedTime*0.6);
	
	vec3 pos = gl_Vertex.xyz;
	vec3 norm = gl_Normal.xyz;
	vec4 p = C*R*vec4(pos, 1);  //4d position that is rotating, in view coords
	vec4 lPos = C*R*lightPos;   //light is rotating with the point, in view coords
	
	// diffuse coefficent //
	vec4 l = normalize(lPos - p);  //vector to the light, normalized
	vec4 n = vec4(norm,0);  //normal vector, unchanged, so should still be normalized 
	n = normalize(C*R*n);  //normals must move with triangle faces
	d = clamp(dot(l,n), 0, 1);  //only valid in range [0,1]
	
	// specular coefficient //
	vec4 camPos = vec4(0);  //camera is always at origin in view coords!
	vec4 v = normalize(camPos-p);  //view vector goes to camera, normalized
	vec4 r = reflect(-l,n);  //light vector points away from surface, but reflect() needs incident, so invert
	s = clamp(dot(v,r), 0, 1);  //only valid in range [0,1]
	s = pow(s, 10);  //exponent controls sharpness of specular highlight
	
	gl_Position = P*p;  //project point into clip space
	fColor = norm;
}