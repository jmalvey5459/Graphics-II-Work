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
	
	drawPostProcessEffects_mrt_fs4x.glsl
	Performs multiple post-processing effects.
*/

#version 410

// ****TO-DO: 
//	1) declare varying to receive texture coordinate
//		- the VS paired with this shader is pass texcoord
//		- since we are drawing a FSQ, this is also screen space
//	2) declare uniform sampler to be processed
//	3) declare any other uniforms used for processing
//	4) sample the original image
//	5) you now have a color... do what you want with it!
//		- pro tip: functions are cool

layout (location = 0) out vec4 rtFragColor0;
layout (location = 1) out vec4 rtFragColor1;
layout (location = 2) out vec4 rtFragColor2;
layout (location = 3) out vec4 rtFragColor3;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE SHADES OF BLUE
	rtFragColor0 = vec4(0.0, 0.0, 0.2, 1.0);
	rtFragColor1 = vec4(0.0, 0.0, 0.4, 1.0);
	rtFragColor2 = vec4(0.0, 0.0, 0.6, 1.0);
	rtFragColor3 = vec4(0.0, 0.0, 0.8, 1.0);
}
