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
	
	postCompositeShading_deferred_fs4x.glsl
	Perform deferred shading and composite with textured objects.
*/

#version 410

// ****TO-DO: 
//	1) declare image inputs (g-buffers)
//	2) declare uniforms for lighting and shading
//		- light structure
//		- light block
//		- light count
//		- textures
//		- other
//	3) declare render targets if drawing individual lighting components
//	4) test if depth buffer is 1; if so, discard (no data for this fragment)
//	5) sample from g-buffers to recover scene data and decode as needed
//	6) calculate and accumulate shading for each light
//	7) finally combine with surface color for Phong model
//	8) output results

in vec2 vPassTexcoord;

out vec4 rtFragColor;

void main()
{
	// DUMMY OUTPUT: all fragments are OPAQUE BLUE
	rtFragColor = vec4(0.0, 0.0, 1.0, 1.0);
}
