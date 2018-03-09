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
	
	passAttribsView_atlas_transform_vs4x.glsl
	Transform position and texture coordinates, and pass all lighting-relevant 
		attributes down the pipeline.
*/

#version 410
//this is the atlas shader
// ****TO-DO: 
//	1) declare all attributes pertinent to lighting and shading
//	2) declare transformation matrices
//	3) declare outbound data
//	4) transform variables appropriately
//	5) copy outbound transformed data

layout (location = 0) in vec4 aPosition;
layout (location = 2) in vec4 aNormal;		// (1)
layout (location = 8) in vec4 aTexcoord;	// (1)	
layout (location = 10) in vec4 aTangent;	// (1)	
layout (location = 11) in vec4 aBitangent;	// (1)	

// uniform mat4 uMVP;
uniform mat4 uMV, uP, uAtlas;

out vbShadingData
{
	vec4 vPosition_eye;
	vec4 vTangent_eye;
	vec4 vBitangent_eye;
	vec4 vNormal_eye;
	vec4 vTexcoord_atlas;		
	
} vPassData;

void main()
{
	// gl_Position = uMV * uP * aPosition;

	// (4)
	vec4 p_eye = uMV * aPosition;
	vec4 N_eye = uMV * vec4(aNormal.xyz, 0.0);
	vec4 T_eye = uMV * vec4(aTangent.xyz, 0.0);
	vec4 B_eye = uMV * vec4(aBitangent.xyz, 0.0);
	vec4 uv = uAtlas * aTexcoord;	

	// (5)
	gl_Position = uP * p_eye;
	vPassData.vPosition_eye = p_eye;
	vPassData.vNormal_eye = N_eye;
	vPassData.vTangent_eye = T_eye;
	vPassData.vBitangent_eye = B_eye;
	vPassData.vTexcoord_atlas = uv;
}
