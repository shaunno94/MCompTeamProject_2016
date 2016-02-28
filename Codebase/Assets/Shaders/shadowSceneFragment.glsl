# version 150 core
# extension GL_EXT_gpu_shader4 : require

uniform sampler2D diffuseTex ;
uniform sampler2D bumpTex ;
uniform samplerCubeShadow shadowTex ; // NEW !

uniform vec4 lightColour ;
uniform vec3 lightPos ;
uniform vec3 cameraPos ;
uniform float lightRadius ;
uniform mat4 lightProj;

in Vertex {
	vec3 colour ;
	vec2 texCoord ;
	vec3 normal ;
	vec3 tangent ;
	vec3 binormal ;
	vec3 worldPos ;
	vec4 shadowProj ; // New !
} IN ;

out vec4 gl_FragColor ;

void main ( void ) {
mat3 TBN = mat3 ( IN.tangent , IN.binormal , IN.normal );
vec3 normal = normalize ( TBN *( texture2D ( bumpTex , IN . texCoord ). rgb * 2.0 - 1.0));

vec4 diffuse = texture2D ( diffuseTex , IN.texCoord );

//temp texture override//
normal = IN.normal;
diffuse.xyz = IN.colour;
//-----//

vec3 incident = normalize ( lightPos - IN.worldPos );
float lambert = max (0.0 , dot ( incident , normal )); // Different !

float dist = length ( lightPos - IN . worldPos );
float atten = 1.0 - clamp ( dist / lightRadius , 0.0 , 1.0);

vec3 viewDir = normalize ( cameraPos - IN . worldPos );
vec3 halfDir = normalize ( incident + viewDir );

float rFactor = max (0.0 , dot ( halfDir , normal )); // Different !
float sFactor = pow ( rFactor , 33.0 );


vec3 abs_position = abs(lightPos - (IN.worldPos + normal * 1.5f));
float fs_z = -max(abs_position.x, max(abs_position.y, abs_position.z));
vec4 clip = lightProj * vec4(0.0, 0.0, fs_z+10, 1.0);
float depth = (clip.z / clip.w) * 0.5 + 0.5;

incident.x *= -1.0f;
//float col = texture(shadowTex, vec4(incident, depth));
lambert *= texture(shadowTex, vec4(incident, depth));




vec3 colour = ( diffuse.rgb * lightColour . rgb );
colour += ( lightColour.rgb * sFactor ) * 0.33;
gl_FragColor = vec4 ( colour * atten * lambert , diffuse . a );
gl_FragColor.rgb += ( diffuse.rgb * lightColour.rgb ) * 0.1;
//gl_FragColor = vec4(col,col, col,1);
}
