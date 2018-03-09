
#version 410

// ****TO-DO: 
//	1) declare inbound data
//	2) declare uniforms for lighting and shading
//		- light structure
//		- light block
//		- light count
//		- textures
//	3) declare render targets isf drawing individual lighting components
//	4) calculate and accumulate shading for each light
//	5) finally combine with surface color for Phong model
//	6) output results

// (1)
in vbShadingData
{	
	vec4 vPosition_eye;
	vec4 vTangent_eye;
	vec4 vBitangent_eye;
	vec4 vNormal_eye;
	vec4 vTexcoord_atlas;
} vPassData;

// (2)
uniform sampler2D uTex_dm;
uniform sampler2D uTex_sm;
uniform int uLightCount;

struct sPointLight
{
	vec4 worldPos;		
	vec4 viewPos;			
	vec4 color;			
	float radius;			
	float radiusInvSq;		
	float pad[2];			
};

uniform ubPointLight
{
	sPointLight light[1024];
};


layout (location = 0) out vec4 rtFragColor;
layout (location = 1) out vec4 rtDiffuseShading;
layout (location = 2) out vec4 rtSpecularShading;

// (4)
void calcDiffuseSpecularCoeffs(
		in vec3 lightPos, in vec3 eyePos,
		in vec3 fragPos, in vec3 fragNormal,
		out float diffuseCoeff, out float specularCoeff,
		out float distanceSq)
{
	vec3 lightVec = (lightPos - fragPos);
	distanceSq = dot(lightVec, lightVec);
	lightVec = normalize(lightVec);
	diffuseCoeff = dot(fragNormal, lightVec);

	vec3 viewVec = (eyePos - fragPos);
	viewVec = normalize(viewVec);
	vec3 reflVec = (diffuseCoeff + diffuseCoeff)*fragNormal - lightVec;
	specularCoeff = dot(viewVec, reflVec);
}


void main()
{

	vec3 p = vPassData.vPosition_eye.xyz;
	vec3 N = normalize(vPassData.vNormal_eye.xyz);

	int i;
	float kd, ks, atten;
	vec3 diffuseShading = vec3(0.0), specularShading = vec3(0.0);

	for(i = 0; i < uLightCount; ++i)
	{
		
		calcDiffuseSpecularCoeffs(
			light[i].viewPos.xyz, vec3(0.0), p, N, kd, ks, atten);
		

		atten = 1.0 - atten * light[i].radiusInvSq;
		atten = max(atten, 0);

		kd = max(kd, 0.0);
		ks = max(ks, 0.0);
		ks *= ks;
		ks *= ks;
		ks *= ks;
		ks *= ks;

		diffuseShading += atten * kd * light[i].color.rgb;
		specularShading += atten * ks * light[i].color.rgb;
	}

	rtDiffuseShading = vec4(diffuseShading, 1.0);
	rtSpecularShading = vec4(specularShading, 1.0);


	vec4 diffuseSample = texture(uTex_dm, vPassData.vTexcoord_atlas.xy);
	vec4 specularSample = texture(uTex_sm, vPassData.vTexcoord_atlas.xy);

	// (5)
	rtFragColor = vec4(diffuseShading * diffuseSample.rgb + specularShading * specularSample.rgb, diffuseSample.a);

}