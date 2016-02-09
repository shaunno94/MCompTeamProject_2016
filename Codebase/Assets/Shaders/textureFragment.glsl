#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec2 texCoord;
} IN;

out vec4 outFragColor;

void main(void)	{
	outFragColor = texture(diffuseTex, IN.texCoord);
}
