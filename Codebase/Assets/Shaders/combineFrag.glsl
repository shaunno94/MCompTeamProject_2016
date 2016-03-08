# version 150 core
uniform sampler2D diffuseTex ;
uniform sampler2D emissiveTex ;
uniform sampler2D specularTex ;

in Vertex {
vec2 texCoord ;
} IN ;

out vec4 outFragColor ;

void main ( void ) {
	vec3 diffuse = texture ( diffuseTex , IN.texCoord ).xyz ;
	vec3 light = texture ( emissiveTex , IN.texCoord ).xyz ;
	vec3 specular = texture ( specularTex , IN.texCoord ).xyz ;

	outFragColor.xyz = diffuse * 0.3; // ambient
	outFragColor.xyz += diffuse * light ; // lambert
	outFragColor.xyz += specular ; // Specular
	outFragColor.a = 1.0;
	
	//gl_FragColor.xyz = diffuse ;
	//gl_FragColor.xyz = light ;
	//gl_FragColor.xyz += specular;
}
