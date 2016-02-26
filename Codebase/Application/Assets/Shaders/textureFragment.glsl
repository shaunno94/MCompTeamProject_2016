#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec2 texCoord;
	vec3 normal ; //added
} IN;

out vec4 outFragColor [2];//added

void main(void)	{
	//outFragColor = texture(diffuseTex, IN.texCoord);
	outFragColor [0] = texture2D ( diffuseTex , IN.texCoord );//added
	outFragColor [1] = vec4 ( IN.normal.xyz * 0.5 + 0.5 ,1.0);//added
}
