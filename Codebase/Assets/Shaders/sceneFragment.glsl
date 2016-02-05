#version 150 core

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 outFragColor;

void main(void)	{
	outFragColor = IN.colour;
	if(useTexture > 0) {
		outFragColor *= texture(diffuseTex, IN.texCoord);
	}
}
