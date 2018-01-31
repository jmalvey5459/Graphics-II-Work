/*
	Copyright 2011-2018 Daniel S. Buckstein

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	animal3D SDK: Minimal 3D Animation Framework
	By Daniel S. Buckstein
	
	drawPhong_fs4x.glsl
	Receive variables for calculating Phong shading, output result.
*/

#version 410

// ****TO-DO: 
//	1) declare varyings to receive data from vertex shader
//	2) declare uniform variables for textures; see demo code for hints
//	3) grab samples from textures
//	4) calculate diffuse and specular coefficients
//	5) calculate Phong shading model 
//	6) assign result to output color
uniform sampler2D uTex_dm;
uniform sampler2D uTex_sm;


in vec2 vPhongPassTexcoord;
in vec3 vPhongPassNormal;
in vec3 vPhongPassLighting;
in vec3 vPhongPassView;

out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE BLUE
	//rtFragColor = vec4(0.0, 0.0, 1.0, 1.0);
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

	//phong
	
	vec3 ambient = vec3(0,0,0.1);
	vec3 phong = diffuse + ambient;
	
	//all together now!
	vec4 earth = texture(uTex_dm, vPhongPassTexcoord);
	vec4 water = texture(uTex_sm, vPhongPassTexcoord);
	earth.rgb *= phong;
	water.rgb *= specular;
	water.a = 0;
	rtFragColor = earth + water;
	
}
