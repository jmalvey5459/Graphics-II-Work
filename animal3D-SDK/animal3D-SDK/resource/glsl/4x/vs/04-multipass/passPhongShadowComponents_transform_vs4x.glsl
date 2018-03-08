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
	
	passPhongShadowComponents_transform_vs4x.glsl
	Pass Phong shading components and shadow coordinate.
*/

#version 410

// ****TO-DO: 
//	1) declare other attributes and uniforms for lighting (Phong)
//	2) declare alternate transformation stack with bias (uniform matrix)
//	3) declare varyings to pass lighting variables
//	4) declare varying to pass shadow coordinate
//	5) calculate and pass values for lighting and shading
//	6) calculate and pass shadow coordinate

layout (location = 0) in vec4 aPosition;

uniform mat4 uMVP;

void main()
{
	gl_Position = uMVP * aPosition;
}
