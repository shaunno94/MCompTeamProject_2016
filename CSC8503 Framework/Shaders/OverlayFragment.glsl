#version 150 core

uniform sampler2D diffuseTex;
uniform vec3 ambientColour;

out vec4 fragColorOut;

void main(void)	{
	fragColorOut = vec4(1.0, 1.0, 1.0, 0.5);
}
