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
	
	drawDiffuse_fs4x.glsl
	Receive variables for calculating diffuse coefficient, output shading.
*/
//This file was modified by jack malvey with permission from the author

#version 410

// ****TO-DO: 
//	1) declare varyings to receive data from vertex shader

in vec3 vPassNormal;
in vec3 vPassLighting;
//	2) calculate diffuse coefficient
//	3) copy diffuse coefficient to output as opaque grayscale color

out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE RED
	//rtFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec3 L = normalize(vPassLighting);
	vec3 N = normalize(vPassNormal);
	float diffuse = dot(N,L);
	rtFragColor = vec4(diffuse, diffuse, diffuse, 1.0);

}
