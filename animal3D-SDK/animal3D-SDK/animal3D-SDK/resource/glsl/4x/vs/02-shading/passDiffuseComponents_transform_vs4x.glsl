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
	
	passDiffuseComponents_transform_vs4x.glsl
	Pass variables pertinent to diffuse shading.
*/

#version 410

// ****TO-DO: 
//	1) declare attribute for normal
//	2) declare uniforms for variables in code pertinent to lighting
//	3) declare varyings to pass relevant data (per-vertex vs. per-fragment)
//	4) pass data to next shader

layout (location = 0) in vec4 aPosition;
layout (location = 2) in vec3 aNormal;

uniform mat4 uMVP;

uniform vec4 uLightPos_obj; 

//This file was modified by jack malvey with permission from the author

//per vertex
//out float vShading;

out vec3 vPassNormal;
out vec3 vPassLighting;

void main()
{
	gl_Position = uMVP * aPosition;

	//normalize ligth vector, dot product for diffuse coefficient 
	//vec3 L = normalize(uLightPos_obj - aPosition).xyz;
	//vec3 N = normalize(aNormal);
	//float diffuse = dot(N,L);
	//^^^^ this is definitely going to be on the quiz on monday
	//vShading = diffuse;
	vPassNormal = aNormal;
	vPassLighting = (uLightPos_obj.xyz - aPosition.xyz);

}

