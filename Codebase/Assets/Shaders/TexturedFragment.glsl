#version 150 core

uniform sampler2D diffuseTex;

in Vertex{
	vec2 texCoord;
} IN;

out vec4 outFragColor;

void main(void){
	vec4 colour = texture(diffuseTex, IN.texCoord);
	outFragColor = colour;
}