#version 450 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

in vec3 position;

out Vertex{
	mat4 inverseProjView;
}OUT;


 void main ( void ) {
	gl_Position = ( projMatrix * viewMatrix * modelMatrix )	* vec4 ( position , 1.0);

	OUT.inverseProjView = inverse ( projMatrix * viewMatrix );
}
