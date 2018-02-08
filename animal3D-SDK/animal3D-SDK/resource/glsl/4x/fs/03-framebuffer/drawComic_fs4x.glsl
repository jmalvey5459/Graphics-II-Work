#version 410

layout (location = 0) out vec4	rtComicBook;


uniform sampler2D uTex_dm;
in vec2 vTexcoord;


void main(){
	//frequency of dots
    float hz = 350.0;
	//rotate dots by 45 degrees for comic print effect
	vec2 rotCoord = vTexcoord;
	rotCoord *=  mat2(0.707, -0.707, 0.707, 0.707);

	//make dots
    vec2 nearest = 2.0*fract(hz * rotCoord) - 1.0;
    float dist = length(nearest);

	//get colors from texture
	vec3 texColors = texture2D(uTex_dm, vTexcoord).rgb;


	//arrange dots based on texture to get the look of the texture
//green looks best
	float radius = sqrt(1.0 - texColors.g);
//	float radius = sqrt(1.0 - texColors.r);
//	float radius = sqrt(1.0 - texColors.b);

	//background colors
	//black looks ugly, you can't see the checker box with white, grey makes an okay background color
    vec3 white = vec3(1.0, 1.0, 1.0);
    vec3 grey = vec3(0.75, 0.75, 0.75);
    vec3 black = vec3(0.0, 0.0, 0.0);
	
	 //get cmyk colors
	 //equations found here:https://bensimonds.com/2013/02/14/halftone-shader/
	float c = 1.0 - texColors.r;
	float m = 1.0 - texColors.g;
	float y = 1.0 - texColors.b;


	float K = min(min(c,m),y);

	float C = ((c-K)/(1.0-K));
	float M = ((m-K)/(1.0-K));
	float Y = ((y-K)/(1.0-K));

	//invert cmyk values
	vec3 CMYK = vec3(1.0 - C, 1.0 - M, 1.0 - Y);

	//combine colors, background, and the dots
	rtComicBook = vec4(mix(CMYK, grey, step(radius, dist)),1);
}
