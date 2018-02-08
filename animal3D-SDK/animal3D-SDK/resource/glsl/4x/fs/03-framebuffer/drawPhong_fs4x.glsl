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
//This file was modified by jack malvey with permission from the author

#version 410



layout (location = 0) out vec4	rtTex;
layout (location = 1) out vec4	rtDiff;
layout (location = 2) out vec4	rtLamb;
layout (location = 3) out vec4	rtPhong;


uniform sampler2D uTex_dm;
uniform sampler2D uTex_sm;


in vec2 vPhongPassTexcoord;
in vec3 vPhongPassNormal;
in vec3 vPhongPassLighting;
in vec3 vPhongPassView;


void main()
{
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
	rtTex = earth;
	rtDiff = new vec4(diffuse,diffuse,diffuse,1);
	rtLamb = new vec4(specular,specular,specular,1);
	vec4 water = texture(uTex_sm, vPhongPassTexcoord);

	earth.rgb *= phong;
	water.rgb *= specular;
	water.a = 0;
	rtPhong = earth + water;
	
}
