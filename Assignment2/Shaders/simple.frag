#version 150

in vec4 smoothColor;
flat in vec4 uniqueColor;

uniform bool pickingEnabled;

out vec4 fragColor;

void main()
{
	if(pickingEnabled) {
		fragColor = uniqueColor;
	} else {
		fragColor = smoothColor;
	}
	
}