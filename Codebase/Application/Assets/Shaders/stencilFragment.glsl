#version 150 core

uniform sampler2D diffuseTex;
uniform bool test;

in Vertex	{
	vec2 texCoord;
} IN;

out vec4 outFragColor;

void main(void)	{
	vec4 value = texture(diffuseTex, IN.texCoord).rgba;
	if(test && value.a != 0.0) {
		discard;
	}
	outFragColor = value;
}
