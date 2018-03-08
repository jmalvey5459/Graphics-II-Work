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
	
	passBiasClipPos_transform_vs4x.glsl
	Transform position to clip and pass biased result down the pipeline.
*/

#version 410

// ****TO-DO: 
//	- nothing

layout (location = 0) in vec4 aPosition;

uniform mat4 uMVP;

out vec4 vPassBiasClipPos;

const mat4 bias = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

void main()
{
	vec4 clipPos = uMVP * aPosition;
	gl_Position = clipPos;
	vPassBiasClipPos = bias * clipPos;
}
