
#version 410


uniform sampler2D uTex_dm;
uniform sampler2D uTexToon_dm;
uniform sampler2D uTexToon_sm;


in vec2 vPhongPassTexcoord;
in vec3 vPhongPassNormal;
in vec3 vPhongPassLighting;
in vec3 vPhongPassView;

out vec4 rtFragColor;
void main()
{
	vec4 diffuseSample = texture(uTex_dm, vPhongPassTexcoord);
	//diffuse
	vec3 L = normalize(vPhongPassLighting);
	vec3 N = normalize(vPhongPassNormal);
	float diffuse = dot(N,L);

	vec3 R = (diffuse + diffuse) *N - L;
	vec3 V = normalize(vPhongPassView);
	float specular = dot(V, R);

	diffuse = max(0.0, diffuse);
	//diffuse = max(diffuse, 1.0);


	vec2 diffRamp = vec2(0.0f);
	diffRamp.x = (diffuse * 0.5 + 0.5);
	
	specular = max(0.0, specular);	
	specular *= specular;
	specular *= specular;
	specular *= specular;
	specular *= specular;
	vec2 specRamp = vec2(0.0f);
	specRamp.x = (specular * 0.5 + 0.5);

	//ramp.x = max(0.0, ramp.x);
	vec4 toon = texture(uTexToon_dm, diffRamp);
	vec4 specularToon = texture(uTexToon_sm,specRamp);
	//toon.a = 0;
	//specularToon.a = 0;

	//diffuseSample*=toon;
/*
	if(diffuse <= 0.0f)
	diffuse = 0.0f;
	else if (diffuse <= 0.25f)
	diffuse = 0.25f;
	else if (diffuse <= 0.5f)
	diffuse = 0.5f;
	else if (diffuse <= 0.75f)
	diffuse = 0.75f;
	else
	diffuse = 1.0f;
*/
	//toon.x = max(0.0, toon.x);
	//specularToon.x = max(0.0, specularToon.x);
	rtFragColor = (diffuseSample);
	rtFragColor *= (toon * specularToon);//;diffuseSample;//
	//rtFragColor.rgb *=  specularToon.x;

	//rtFragColor.a = 1;

//rtFragColor = diffuseSample;
//rtFragColor.rgb *= diffuse;
}