# version 150 core

uniform sampler2D depthTex ;
uniform sampler2D normTex ;

uniform sampler2DShadow shadowTex;
uniform mat4 shadowBias ;

uniform vec2 pixelSize ;
uniform vec3 cameraPos ;

uniform float lightRadius ;
uniform vec3 lightPos ;
uniform vec4 lightColour ;

in Vertex {
	mat4 inverseProjView ;
	//vec4 shadowProj;
} IN;
out vec4 colourOut [2];

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
	
	float shadow = 1.0; 
	
	vec4 shadowProj = ( shadowBias * vec4 ( pos +( normal *1.5) ,1));

	if( shadowProj . w > 0.0) { 
		shadow = textureProj ( shadowTex , shadowProj );
	}

	lambert *= shadow ;
	sFactor *= shadow;
	
	colourOut [0] = vec4 ( lightColour . xyz * lambert * atten , 1.0);
	colourOut [1] = vec4 ( lightColour . xyz * sFactor * atten *0.33 ,1.0);
	//colourOut [1].rgb = normal.rgb;
}