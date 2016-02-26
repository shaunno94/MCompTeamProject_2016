#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec2 texCoord;
	vec3 normal ; //added
} IN;

out vec4 outFragColor [2];//added

void main(void)	{
	vec4 colour = texture(diffuseTex, IN.texCoord);
	
	if (colour.a < 0.2) {
		discard;
	}
	
	//colour = vec4(colour.a);
	outFragColor [0] = colour;//added
	outFragColor [1] = vec4 ( IN.normal.xyz * 0.5 + 0.5 ,1.0);//added
}
