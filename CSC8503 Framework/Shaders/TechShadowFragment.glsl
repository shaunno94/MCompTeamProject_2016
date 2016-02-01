#version 150 core

uniform sampler2D diffuseTex;
uniform vec3 ambientColour;

in Vertex	{
	vec3 worldPos;
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
} IN;

out vec4 fragColorOut;

void main(void)	{ 
//Colour Computations
	vec4 texColour  = texture(diffuseTex, IN.texCoord);
	vec3 diffColour = IN.colour.rgb * texColour.rgb;
    vec3 col = ambientColour * diffColour;
	
//Output Final Fragment Colour
	fragColorOut.rgb 	= col;
	fragColorOut.a 		= IN.colour.a * texColour.a;
}