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
	
	passthru_transform_vs4x.glsl
	Pass-thru GLSL vertex shader. Outputs transformed position attribute.

	**DO NOT MODIFY THIS FILE**
	Certifcate of Authenticity:
	This file was modified by Jack Malvey with permission from author.

*/

#version 410

layout (location = 0) in vec4 aPosition;

uniform mat4 uMVP;

void main()
{
	//transform input position by MVP matrix 
	gl_Position = uMVP * aPosition;
}