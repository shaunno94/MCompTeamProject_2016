#version 150 core

uniform sampler2D diffuseTexture;

in Vertex	{
	vec2 texCoord;
} IN;

out vec4 outFragColor;

void main(void)	{
	outFragColor = texture(diffuseTexture, IN.texCoord);
}
