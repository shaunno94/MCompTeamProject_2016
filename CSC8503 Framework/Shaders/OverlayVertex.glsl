#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform vec4 nodeColour;

in  vec3 position;

void main(void)	{
	gl_Position		= projMatrix * modelMatrix * vec4(position, 1.0);
}
