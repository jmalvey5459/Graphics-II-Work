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
	
	drawPhongShadow_fs4x.glsl
	Performs shadow mapping.
*/

#version 410

// ****TO-DO: 
//	1) declare varyings to receive lighting and shading data
//	2) declare varying to receive shadow coordinate
//	3) declare uniforms for surface texture samplers
//	4) declare uniform for shadow map
//	5) shadow mapping: 
//		a) convert shadow coord to screen space
//		b) use new coordinate to sample shadow map
//			- may need to clamp their range beforehand
//		c) perform shadow test
//	6) implement lighting model (Phong)
//	7) respond to shadow test appropriately

out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE GREEN
	rtFragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
