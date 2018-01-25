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
	
	passColor_transform_vs4x.glsl
	Transform position attribute and pass color attribute down the pipeline.

	**DO NOT MODIFY THIS FILE**
	
	Certifcate of Authenticity:
	This file was modified by Jack Malvey with permission from author.

*/

#version 410

// ****TO-DO: 
//	2) correctly transform input position by MVP matrix
//	3) declare attribute for vertex color input
//	4) declare varying to pass color input to fragment shader
//	5) assign vertex color input to varying

layout (location = 0) in vec4 aPosition;
//tried 1, 4, 8, 0 (this one was close to what i wanted), 2 then finally 3. 3 works. 
layout (location = 3) in vec4 vColor;
out vec4 vColorOut;
uniform mat4 uMVP;


void main()
{
	gl_Position = uMVP * aPosition;
	vColorOut = vColor;
}
