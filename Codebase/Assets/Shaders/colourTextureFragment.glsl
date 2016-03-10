#version 150 core

uniform sampler2D diffuseTex;
uniform vec4 colour;

in Vertex	{
	vec2 texCoord;
	vec3 normal; //added

} IN;

out vec4 outFragColor[2];//added

void main(void)	{
	vec4 textureColour = texture(diffuseTex, IN.texCoord);
	
	if (textureColour.a < 0.2) {
		discard;
	}
	
	outFragColor[0] = textureColour;//added
	outFragColor[1] = vec4 (IN.normal.xyz * 0.5 + 0.5 ,1.0);//added
	
	outFragColor[0] = outFragColor[0] * colour;
}
