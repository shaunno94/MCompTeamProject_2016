#version 150 core

in Vertex	{
	vec4 colour;
} IN;

out vec4 outFragColor;

void main(void)	{
	outFragColor = IN.colour;
}
