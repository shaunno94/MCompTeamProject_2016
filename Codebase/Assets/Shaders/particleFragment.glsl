#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec2 texCoord;
	vec3 normal; 
} IN;

out vec4 outFragColor[2];

void main(void)	{
	vec4 colour = texture(diffuseTex, IN.texCoord);
	
	if (colour.a < 0.2) {
		discard;
	}
	colour.a *= 0.2;

	outFragColor[0] = colour;
	outFragColor[1] = vec4 (IN.normal.xyz * 0.5 + 0.5 ,1.0);
}
