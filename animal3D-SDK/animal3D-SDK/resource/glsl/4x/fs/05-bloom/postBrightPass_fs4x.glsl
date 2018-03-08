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
	
	postBrightPass_fs4x.glsl
	Implements HDR bright pass filter.
*/

#version 410

// ****TO-DO: 
//	1) declare uniform for image to process
//	2) declare uniform for HDR range (if using)
//	3) define and test function(s) for simple bright area highlighting
//		e.g. luminance, relative luminance
//	4) define and test function(s) for HDR bright pass
//		e.g. procedural tone mapping, may use above function(s)

in vec2 vPassTexcoord;

uniform sampler2D uTex_dm;

out vec4 rtFragColor;

void main()
{
	vec4 texSample = texture(uTex_dm, vPassTexcoord);
	rtFragColor = texSample;
}
