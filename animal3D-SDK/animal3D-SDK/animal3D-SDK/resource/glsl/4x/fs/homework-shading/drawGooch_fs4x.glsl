/*
Jack Malvey - 1005854
Certificate of Authenticity: “I certify that this work is
entirely my own. The assessor of this project may reproduce this project
and provide copies to other academic staff, and/or communicate a copy of
this project to a plagiarism-checking service, which may retain a copy of the
project on its database.”
*/
#version 410


uniform sampler2D uTex_dm;
uniform sampler2D uTex_sm;


in vec2 vPhongPassTexcoord;
in vec3 vPhongPassNormal;
in vec3 vPhongPassLighting;
in vec3 vPhongPassView;

out vec4 rtFragColor;

void main()
{
	
	const vec3 cold = vec3(0.0, 0.0, 0.1);
	const vec3 hot = vec3 (0.8, 0.25, 0.0);
	//diffuse
	vec3 L = normalize(vPhongPassLighting);
	vec3 N = normalize(vPhongPassNormal);
	float diffuse = dot(N,L);

	//reflection
	vec3 R = (diffuse + diffuse) * N - L;

	//specular
	vec3 V = normalize(vPhongPassView);
	float specular = dot(V, R);


	diffuse = max(0.0, diffuse);
	specular = max(0.0, specular);
	
	specular *= specular;
	specular *= specular;
	specular *= specular;
	specular *= specular;


	//phong
	
	vec3 ambient = vec3(0,0,0.1);
	
	//lets just do gooch instead
	vec3 ice = (diffuse * 0.75) + cold;
	vec3 fire = (diffuse * 0.75) + hot;
	vec3 gooch = mix(ice, fire, diffuse);
	vec3 phong = gooch ;

	//all together now!
	vec4 earth = texture(uTex_dm, vPhongPassTexcoord);
	vec4 water = texture(uTex_sm, vPhongPassTexcoord);
	earth.rgb *= phong;
	water.rgb *= specular;
	water.a = 0;
	rtFragColor = earth + water;
	
}