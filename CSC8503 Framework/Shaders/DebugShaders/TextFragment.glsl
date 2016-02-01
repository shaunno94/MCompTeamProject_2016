#version 150 core

uniform sampler2D fontTex;

in Vertex	{
	vec4 colour;
	vec2 texCoords;
} IN;

out vec4 fragColorOut;

void main(void)	{
	fragColorOut = texture(fontTex, IN.texCoords) * IN.colour;
	
	if (fragColorOut.w < 0.1f)
	{
		discard;
	}
}