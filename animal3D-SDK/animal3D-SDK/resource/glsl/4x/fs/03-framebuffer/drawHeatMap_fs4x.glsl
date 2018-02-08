#version 410

layout (location = 0) out vec4	rtFragColor;


uniform sampler2D uTex_dm;
in vec2 vTexcoord;


void main(){

    float hz = 1000.0;
	vec2 rotCoord = vTexcoord;
	rotCoord *=  mat2(0.707, -0.707, 0.707, 0.707);
    vec2 nearest = 2.0*fract(hz * rotCoord) - 1.0;
    float dist = length(nearest);

	vec3 texColors = texture2D(uTex_dm, vTexcoord).rgb;



//green looks best
	float radius = sqrt(1.0 - texColors.g);
//	float radius = sqrt(1.0 - texColors.r);
//	float radius = sqrt(1.0 - texColors.b);


    vec3 white = vec3(1.0, 1.0, 1.0);
    vec3 black = vec3(0.0, 0.0, 0.0);

    vec3 fragcolor = mix(black, white, step(radius,dist));
	//smoothstep((radius - dFdx(dist)), (radius + dFdx(dist)), dist));
    rtFragColor = vec4(fragcolor, 1.0);

/*
	vec3 magentaCol = vec3(1.0, 0.0, 1.0);
    vec3 cyanCol = vec3(0.0, 1.0 ,1.0);
    vec3 yellowCol = vec3(1.0, 1.0, 0.0);

	 vec4 earth =  texture2D(uTex_dm, vTexcoord);
   float reds = smoothstep(0.0, 1.0, earth.r);
   float greens = smoothstep(0.0, 1.0, earth.g);
    float blues = smoothstep(0.0, 1.0, earth.b);

	float c = 1.0 - reds;
	float m = 1.0 - greens;
	float y = 1.0 - blues;

//float c = 1.0 - earth.r;
//float m = 1.0 - earth.g; 
//float y = 1.0 - earth.b;

	float K = min(min(c,m),min(y,m));

	float C = ((c-K)/(1.0-K));
	float M = ((m-K)/(1.0-K));
	float Y = ((y-K)/(1.0-K));




	earth.rgb = vec3(1-C, 1-M, 1-Y);
	*/
	//rtFragColor = earth;
}
