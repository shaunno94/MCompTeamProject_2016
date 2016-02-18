# version 150 core
# extension GL_EXT_gpu_shader4 : require

uniform sampler2D depthTex ;
uniform sampler2D normTex ;

uniform samplerCubeShadow shadowTex;
uniform mat4 lightProj;

uniform vec2 pixelSize ;
uniform vec3 cameraPos ;

uniform float lightRadius ;
uniform vec3 lightPos ;
uniform vec4 lightColour ;

in Vertex {
	mat4 inverseProjView ;
	//vec4 shadowProj;
} IN;
out vec4 gl_FragColor [2];

void main ( void ) {
	vec3 pos = vec3 (( gl_FragCoord . x * pixelSize . x ) ,
	( gl_FragCoord . y * pixelSize . y ) , 0.0);
	pos . z = texture ( depthTex , pos . xy ). r ;

	vec3 normal = normalize ( texture ( normTex , pos . xy ). xyz *2.0 - 1.0);

	vec4 clip = IN.inverseProjView * vec4 ( pos * 2.0 - 1.0 , 1.0);
	pos = clip . xyz / clip . w ;

	float dist = length ( lightPos - pos );
	float atten = 1.0 - clamp ( dist / lightRadius , 0.0 , 1.0);

	if( atten == 0.0) {
		discard ;
	}

	vec3 incident = normalize ( lightPos - pos );
	vec3 viewDir = normalize ( cameraPos - pos );
	vec3 halfDir = normalize ( incident + viewDir );

	float lambert = clamp ( dot ( incident , normal ) ,0.0 ,1.0);
	float rFactor = clamp ( dot ( halfDir , normal ) ,0.0 ,1.0);
	float sFactor = pow ( rFactor , 33.0 );
	
	vec3 abs_position = abs(lightPos - (pos + normal * 1.5f));
	float fs_z = -max(abs_position.x, max(abs_position.y, abs_position.z));
	vec4 lclip = lightProj * vec4(0.0, 0.0, fs_z+10, 1.0);
	float depth = (lclip.z / lclip.w) * 0.5 + 0.5;

	incident.x *= -1.0f;
	float shadow = texture(shadowTex, vec4(incident, depth));
	
	lambert *= shadow ;
	sFactor *= shadow;
	
	gl_FragColor [0] = vec4 ( lightColour . xyz * lambert * atten , 1.0);
	gl_FragColor [1] = vec4 ( lightColour . xyz * sFactor * atten *0.33 ,1.0);
	//gl_FragColor [1].rgb = normal.rgb;
}