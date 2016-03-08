# version 150 core
uniform sampler2D diffuseTex ;
uniform sampler2D emissiveTex ;
uniform sampler2D specularTex ;

in Vertex {
vec2 texCoord ;
} IN ;

out vec4 colourOut ;

void main ( void ) {
	vec3 diffuse = texture ( diffuseTex , IN.texCoord ).xyz ;
	vec3 light = texture ( emissiveTex , IN.texCoord ).xyz ;
	vec3 specular = texture ( specularTex , IN.texCoord ).xyz ;

	colourOut.xyz = diffuse * 0.3; // ambient
	colourOut.xyz += diffuse * light ; // lambert
	colourOut.xyz += specular ; // Specular
	colourOut.a = 1.0;
	
	//colourOut.xyz = diffuse ;
	//colourOut.xyz = light ;
	//colourOut.xyz += specular;
}
