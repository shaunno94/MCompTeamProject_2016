# version 150 core

uniform samplerCube cubeTex ;
uniform vec3 cameraPos ;

in Vertex {
	vec3 normal ;
} IN ;
out vec4 outFragColor ;

void main ( void ) {
	outFragColor = texture ( cubeTex , normalize ( IN.normal ));
}