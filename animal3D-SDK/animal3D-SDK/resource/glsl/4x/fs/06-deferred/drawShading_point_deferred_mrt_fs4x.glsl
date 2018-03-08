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
	
	drawShading_point_deferred_mrt_fs4x.glsl
	Perform shading calculations while rendering point light volume, output 
		to MRT.
*/

#version 410

// ****TO-DO: 
//	1) declare inputs: biased clip position and instance ID
//	2) declare image inputs (g-buffers)
//	3) declare uniforms for lighting and shading
//		- light structure
//		- light block
//		- light count
//		- other
//	4) declare render targets if drawing individual lighting components
//	5) test if depth buffer is 1; if so, discard (no data for this fragment)
//	6) sample from g-buffers to recover scene data and decode as needed
//	7) calculate and accumulate shading for each light
//	8) output results

out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE GREEN
	rtFragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
