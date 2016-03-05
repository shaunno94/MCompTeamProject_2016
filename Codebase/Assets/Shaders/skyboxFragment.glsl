#version 450 core

uniform samplerCube cubeTex;

in Vertex {
	vec3 normal;
} IN;
out vec4 FragColour;

void main (void) 
{
	FragColour = texture(cubeTex, normalize(IN.normal));
}