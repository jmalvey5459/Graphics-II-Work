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
	
	drawAttribs_mrt_fs4x.glsl
	Draw all attributes to MRT as color.
*/

#version 410

// ****TO-DO: 
//	1) declare varying block to receive attribute data [x]
//	2) declare MRT[x]
//	3) copy inbound data to outbound render targets (might need to compress)[x]


//goes to each of the demo modes in order
layout (location = 0	) out vec4	rtPosition;
layout (location = 1	) out vec4	rtNormal;
layout (location = 2	) out vec4	rtColor;
layout (location = 3	) out vec4	rtTexcoord;
layout (location = 4	) out vec4	rtTagent;
layout (location = 5	) out vec4	rtBitangent;
layout (location = 6	) out vec4	rtBlendWeight;
layout (location = 7	) out vec4	rtBlendIndices;

in vbPassDataBlock
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


	//thomas told me about the *0.5 + 0.5 clamp being needed for this
	rtPosition	   = vec4(normalize(vPassData.vPosition.rgb *0.5)+0.5,1.0);
	rtNormal	   = vec4(normalize(vPassData.vNormal*0.5)+0.5,1.0);
	rtColor		   = vec4(vPassData.vColor,1.0);
	rtTexcoord	   = vec4(vPassData.vTexcoord,0.0,1.0);
	rtTagent	   = vec4(normalize(vPassData.vTagent*0.5)+0.5,1.0);
	rtBitangent    = vec4(normalize(vPassData.vBitangent*0.5)+0.5,1.0);
	rtBlendWeight  = vec4(vPassData.vBlendWeight,1.0);
	rtBlendIndices = vec4(normalize(vPassData.vBlendIndices*0.5)+0.5,1.0);
}