#version 150 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
uniform mat4 invModelMat;

in  vec3 position;
in  vec2 texCoord;
in vec3 normal ; //added

out Vertex {
	vec2 texCoord;
	vec3 normal ; //added
} OUT;

void main(void)	
{
	mat3 normalMatrix = mat3(invModelMat);
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;

	gl_Position = mvp * vec4(position, 1.0);
	OUT.texCoord = texCoord;
	OUT.normal = normalize(normalMatrix * normalize(normal));
}
