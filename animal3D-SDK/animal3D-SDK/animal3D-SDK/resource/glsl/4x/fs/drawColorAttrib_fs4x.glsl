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
	
	drawColorAttrib_fs4x.glsl
	Draw color attribute passed from prior stage as varying.

	**DO NOT MODIFY THIS FILE**
	
	Certifcate of Authenticity:
	This file was modified by Jack Malvey with permission from author.

*/

#version 410

// ****TO-DO: 
//	1) declare varying to receive input vertex color from vertex shader
//	2) assign vertex color to output color

out vec4 rtFragColor;
in vec4 vColorOut;

void main()
{
// default red, the entire dimension indicator thing is red so im obviously doing something wrong with the varying color
	rtFragColor = vec4(1.0, 0.0, 0.0, 1.0);
	rtFragColor = vColorOut;
}