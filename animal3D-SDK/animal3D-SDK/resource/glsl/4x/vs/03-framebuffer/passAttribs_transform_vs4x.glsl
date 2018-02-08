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
	
	passAttribs_transform_vs4x.glsl
	Transform position and pass all attributes down the pipeline.
*/

#version 410

// ****TO-DO: 
//	1) declare attributes [x]
//	2) declare varying block to pass attribute data out [x]
//	3) copy all attributes to the varying structure to be passed [x]


//nvidya standard locations
layout (location = 0) in vec4 aPosition;
layout (location = 1) in vec3  aBlendWeight;
layout (location = 2) in vec3  aNormal;
layout (location = 3) in vec3  aColor;
layout (location = 8) in vec2  aTexcoord;
layout (location = 10) in vec3 aTagent;
layout (location = 11) in vec3 aBitangent;
layout (location = 12) in vec3 aBlendIndices;


uniform mat4 uMVP;

//missed class when we went over this, gabe explained it to me
out vbPassDataBlock
{
	vec4 vPosition;
	vec3 vNormal;
	vec3 vColor;
	vec2 vTexcoord;
	vec3 vTagent;
	vec3 vBitangent;
	vec3 vBlendWeight;
	vec3 vBlendIndices;
} vPassData;

void main()
{
	gl_Position = uMVP * aPosition;

	//setting data to varyings to pass
	vPassData.vPosition = aPosition;
	vPassData.vNormal = aNormal;
	vPassData.vColor = aColor;
	vPassData.vTexcoord = aTexcoord;
	vPassData.vTagent = aTagent;
	vPassData.vBitangent = aBitangent;
	vPassData.vBlendWeight = aBlendWeight;
	vPassData.vBlendIndices = aBlendIndices;
}		