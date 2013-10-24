#version 150

in vec2 pos;
in vec3 color;
in uint identity;

smooth out vec4 smoothColor;
uniform float time;
flat out vec4 uniqueColor;

vec4 getColor(uint i)
{
	vec4 parts;
	parts.x = float( (i)     & 0x000000ffu) * 0.0039215686274509803921568627451; //the giant float is about 1.0f/255.0f
	parts.y = float( (i>>8u)  & 0x000000ffu) * 0.0039215686274509803921568627451;
	parts.z = float( (i>>16u) & 0x000000ffu) * 0.0039215686274509803921568627451;
	parts.w = float( (i>>24u) & 0x000000ffu) * 0.0039215686274509803921568627451;
	return parts;
}

void main()
{
	vec4 parts = getColor(identity);

	if(identity == 500u || identity == 450u) {
		float x = pos.x * cos(time);
		vec2 newPos = vec2(x, pos.y);
		gl_Position = vec4(newPos, 0, 1);
	} else {
		gl_Position = vec4(pos, 0, 1);
	}	


	//smoothColor = parts;
	smoothColor = vec4(color.xy, color.z, 1);
	uniqueColor = parts;
}