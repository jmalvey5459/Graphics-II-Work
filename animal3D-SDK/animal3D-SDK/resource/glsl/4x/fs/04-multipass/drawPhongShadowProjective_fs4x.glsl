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
	
	drawPhongShadowProjective_fs4x.glsl
	Performs shadow mapping and projective texturing.
*/

#version 410

// ****TO-DO: 
//	- all steps from projective texturing and shadow mapping
//	- areas that pass the shadow test should be projected upon, 
//		areas in shadow should not (think of a projector)

out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE YELLOW
	rtFragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
