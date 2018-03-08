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
	
	drawGBuffers_mrt_fs4x.glsl
	Output shading-relevant attributes from vertex shader to MRT.
*/

#version 410

// ****TO-DO: 
//	1) declare inbound data
//	2) declare render targets
//	3) transform inbound data appropriately
//		- need to consider framebuffer precision
//		- compress and encode values that cannot be stored as-is
//	4) copy to render targets

out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE RED
	rtFragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
