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

// ****TO-DO: 
//	1) declare all attributes pertinent to lighting and shading
//	2) declare transformation matrices
//	3) declare outbound data
//	4) transform variables appropriately
//	5) copy outbound transformed data

layout (location = 0) in vec4 aPosition;

uniform mat4 uMVP;

void main()
{
	gl_Position = uMVP * aPosition;
}
